/* SPDX-License-Identifier: MIT
 * Adapted from ai-passport-bambu-monitor; see docs/architecture/upstream.md.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Validate a complete report without allocating a full JSON tree, merge a delta,
// then publish a coherent snapshot. False leaves the previous state untouched.
bool bambu_report_apply(const char *json, size_t length, uint32_t now_ms);
// Provisioning runs with the monitor stopped. Validate without publishing state.
bool bambu_report_validate(const char *json, size_t length);
