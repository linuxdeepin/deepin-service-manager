// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RUSTPLUGINABI_H
#define RUSTPLUGINABI_H

#include <cstddef>
#include <cstdint>

constexpr std::uint32_t DSM_RUST_PLUGIN_ABI_VERSION = 1;

enum class DSMRustBusType : std::uint32_t {
    System = 0,
    Session = 1,
};

struct DSMRustPluginContextV1
{
    std::uint32_t abiVersion;
    std::uint32_t structSize;
    DSMRustBusType busType;
    std::uint32_t reserved;
    const char *serviceName;
    std::size_t serviceNameLength;
};

using DSMRustStartV1 = std::int32_t (*)(const DSMRustPluginContextV1 *context, void **pluginHandle);
using DSMRustStopV1 = std::int32_t (*)(void *pluginHandle);

#endif // RUSTPLUGINABI_H
