// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include  "utils.h"

#include <QtEndian>
#include <QFile>
#include <QLibrary>
#include <QLoggingCategory>

#ifdef ELF_QT_VER_CHECK
#include <elf.h>
#endif // ELF_QT_VER_CHECK

#include <cstring>
#include <sys/prctl.h>

Q_LOGGING_CATEGORY(dsm_utils, "[Utils]")

void setProcessName(int argc, char **argv, const char *title)
{
    qCDebug(dsm_utils) << "Setting process name to:" << title;

    size_t titleLen = strlen(title);
    size_t argvMaxLen = argv[argc - 1] + strlen(argv[argc - 1]) - argv[0];
    if (titleLen >= argvMaxLen) {
        qCWarning(dsm_utils) << "Process name too long, truncating:" << titleLen << ">" << (argvMaxLen - 1);
        strncpy(argv[0], title, argvMaxLen - 1);
        argv[0][argvMaxLen - 1] = '\0';
    } else {
        strcpy(argv[0], title);
        memset(argv[0] + titleLen, '\0', argvMaxLen - titleLen);
    }

    if (prctl(PR_SET_NAME, title, 0, 0, 0) != 0) {
        qCWarning(dsm_utils) << "Failed to set process name via prctl:" << strerror(errno);
    } else {
        qCDebug(dsm_utils) << "Process name set successfully to:" << title;
    }
}

bool checkLibraryQtVersion(const QString &soPath)
{
    QVersionNumber libraryQtVersion = qtVersionFromSo(soPath);
    QVersionNumber expectedQtVersion = QVersionNumber::fromString(QString::fromUtf8(&USE_QT_VERSION_MAJOR));

    auto checkQtVersion = [](const QVersionNumber &libraryQtVersion, const QVersionNumber &expectedQtVersion) -> bool {
        if (libraryQtVersion.majorVersion() == expectedQtVersion.majorVersion()) {
            return true;
        }

        qCWarning(dsm_utils) << "This library links against Qt" << libraryQtVersion.toString()
                             << "but expected Qt" << expectedQtVersion.majorVersion();
        return false;
    };

    // 优先从 so 文件的 ELF 头解析 Qt 版本信息，避免装载（dlopen）动态库
    if (!libraryQtVersion.isNull()) {
        return checkQtVersion(libraryQtVersion, expectedQtVersion);
    }

    QLibrary library(soPath);
    if (!library.load()) {
        qCWarning(dsm_utils) << "Error opening library:" << soPath << library.errorString();
        return false;
    }

    // 尝试解析Qt版本相关的符号
    using QVersionFunc = const char* (*)();
    auto qVersionFunc = reinterpret_cast<QVersionFunc>(library.resolve("qVersion"));
    if (!qVersionFunc) {
        qCWarning(dsm_utils) << "This library does not appear to link against Qt.";
        return false;
    }

    // 如果找到了qVersion符号，那么这个库确实链接了Qt
    const char *qtVersion = qVersionFunc();
    qCInfo(dsm_utils) << "Library Qt version:" << qtVersion;

    libraryQtVersion = QVersionNumber::fromString(QString::fromUtf8(qtVersion));

    return checkQtVersion(libraryQtVersion, expectedQtVersion);
}

/*
 * Parse version from so file by reading `.qtversion` section in ELF header.
 *
 * Return a null QVersionNumber if failed to parse Qt version from ELF header.
 *
 * Following paragraph is quoted from qtbase/src/corelib/global/qversiontagging.h
 *
 * It's also possible to inspect which version is required by decoding the
 * .qtversion section. The second pointer-sized variable is the required
 * version, for example, for Qt 6.4.1:
 *
 *      Hex dump of section [18] '.qtversion', 16 bytes at offset 0x1ee48:
 *        0x00000000 b0ffffff ffffffff 01040600 00000000 ................
 *                                     ^^^^^^^^ ^^^^^^^^
 */
QVersionNumber qtVersionFromSo(const QString &path)
{
#ifdef ELF_QT_VER_CHECK
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << path;
        return {};
    }

    Elf64_Ehdr ehdr;
    if (f.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr)) != sizeof(ehdr))
        return {};
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0)
        return {};

    const bool is64 = (ehdr.e_ident[EI_CLASS] == ELFCLASS64);
    const bool isLE = (ehdr.e_ident[EI_DATA] == ELFDATA2LSB);
    const size_t ptrSize = is64 ? 8 : 4;

    // 读取节头表
    f.seek(ehdr.e_shoff);
    QVector<Elf64_Shdr> sections(ehdr.e_shnum);
    if (f.read(reinterpret_cast<char*>(sections.data()), sizeof(Elf64_Shdr) * ehdr.e_shnum)
        != qint64(sizeof(Elf64_Shdr) * ehdr.e_shnum))
        return {};

    // 读取 section 名称表
    const Elf64_Shdr &shstr = sections[ehdr.e_shstrndx];
    QByteArray shstrtab(shstr.sh_size, Qt::Uninitialized);
    f.seek(shstr.sh_offset);
    f.read(shstrtab.data(), shstr.sh_size);

    // 查找 ".qtversion" 节
    for (const Elf64_Shdr &sh : sections) {
        const char *secName = shstrtab.constData() + sh.sh_name;
        if (qstrcmp(secName, ".qtversion") != 0)
            continue;

        QByteArray sec(sh.sh_size, Qt::Uninitialized);
        f.seek(sh.sh_offset);
        f.read(sec.data(), sh.sh_size);

        if (size_t(sec.size()) < ptrSize * 2)
            return {};

        // 版本号在第二个指针的位置
        quint32 verRaw = 0;
        memcpy(&verRaw, sec.constData() + ptrSize, 4); // 只取低4字节
        quint32 ver = isLE ? qFromLittleEndian(verRaw) : qFromBigEndian(verRaw);

        int major = (ver >> 16) & 0xFF;
        int minor = (ver >> 8)  & 0xFF;
        int patch = ver & 0xFF;

        return QVersionNumber(major, minor, patch);
    }
#else // ELF_QT_VER_CHECK
    Q_UNUSED(path);
#endif // ELF_QT_VER_CHECK
    return {};
}
