// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

use core::ffi::{c_char, c_void};
use std::mem::size_of;
use std::panic::{AssertUnwindSafe, catch_unwind};
use std::slice;
use std::str;
use zbus::blocking::{Connection, connection};

const ABI_VERSION: u32 = 1;
const SYSTEM_BUS: u32 = 0;
const SESSION_BUS: u32 = 1;

#[repr(C)]
pub struct PluginContextV1 {
    abi_version: u32,
    struct_size: u32,
    bus_type: u32,
    reserved: u32,
    service_name: *const c_char,
    service_name_length: usize,
}

struct PluginState {
    connection: Option<Connection>,
}

struct DemoService;

#[zbus::interface(name = "org.deepin.service.rust.demo1")]
impl DemoService {
    fn hello(&self) -> String {
        "World".to_owned()
    }
}

fn service_name(context: &PluginContextV1) -> Result<&str, i32> {
    if context.service_name.is_null() {
        return Err(-1);
    }

    let bytes = unsafe {
        slice::from_raw_parts(
            context.service_name.cast::<u8>(),
            context.service_name_length,
        )
    };
    str::from_utf8(bytes).map_err(|error| {
        eprintln!("invalid service name: {error}");
        -1
    })
}

fn start_plugin(context: &PluginContextV1) -> Result<PluginState, i32> {
    if context.abi_version != ABI_VERSION
        || context.struct_size as usize != size_of::<PluginContextV1>()
    {
        eprintln!("unsupported Rust plugin ABI");
        return Err(-1);
    }

    let name = service_name(context)?;
    let builder = match context.bus_type {
        SYSTEM_BUS => connection::Builder::system(),
        SESSION_BUS => connection::Builder::session(),
        value => {
            eprintln!("unsupported D-Bus type: {value}");
            return Err(-1);
        }
    }
    .map_err(|error| {
        eprintln!("failed to create D-Bus builder: {error}");
        -1
    })?;

    let connection = builder
        .name(name)
        .map_err(|error| {
            eprintln!("failed to configure D-Bus name: {error}");
            -1
        })?
        .serve_at("/org/deepin/service/rust/demo1", DemoService)
        .map_err(|error| {
            eprintln!("failed to configure D-Bus object: {error}");
            -1
        })?
        .build()
        .map_err(|error| {
            eprintln!("failed to start Rust D-Bus service: {error}");
            -1
        })?;

    Ok(PluginState {
        connection: Some(connection),
    })
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn DSMRustStartV1(
    context: *const PluginContextV1,
    plugin_handle: *mut *mut c_void,
) -> i32 {
    if context.is_null() || plugin_handle.is_null() {
        return -1;
    }

    match catch_unwind(AssertUnwindSafe(|| {
        let state = start_plugin(unsafe { &*context })?;
        let handle = Box::into_raw(Box::new(state)).cast::<c_void>();
        unsafe {
            *plugin_handle = handle;
        }
        Ok::<(), i32>(())
    })) {
        Ok(Ok(())) => 0,
        Ok(Err(error)) => error,
        Err(_) => -1,
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn DSMRustStopV1(plugin_handle: *mut c_void) -> i32 {
    if plugin_handle.is_null() {
        return -1;
    }

    match catch_unwind(AssertUnwindSafe(|| {
        let mut state = unsafe { Box::from_raw(plugin_handle.cast::<PluginState>()) };
        if let Some(connection) = state.connection.take() {
            connection.close().map_err(|error| {
                eprintln!("failed to close Rust D-Bus connection: {error}");
                -1
            })?;
        }
        Ok::<(), i32>(())
    })) {
        Ok(Ok(())) => 0,
        Ok(Err(error)) => error,
        Err(_) => -1,
    }
}
