/*
 * Copyright (C) 2019 ~ 2022 Uniontech Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng@uniontech.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CLIPBOARD_PLUGIN_H
#define CLIPBOARD_PLUGIN_H

#include "clipboard_plugin_global.h"

typedef void (*UnloadFun)(const char *);

extern "C"  CLIPBOARDSHARED_EXPORT bool Start();
extern "C"  CLIPBOARDSHARED_EXPORT bool Stop();
extern "C"  CLIPBOARDSHARED_EXPORT const char *Info();
extern "C"  CLIPBOARDSHARED_EXPORT void UnloadCallBack(UnloadFun fun);

#endif // CLIPBOARD_PLUGIN_H
