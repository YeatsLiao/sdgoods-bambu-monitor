/* SPDX-License-Identifier: MIT
 * Adapted from ai-passport-bambu-monitor; see docs/architecture/upstream.md.
 */
#pragma once
#include "bambu_state.h"
// Overview, ordinary AMS, HT (external once on first HT), details.
enum {BAMBU_PAGE_OVERVIEW=-1,BAMBU_PAGE_EXTERNAL=-2,BAMBU_PAGE_DETAILS=-3};
int bambu_pages_count(const bambu_state_t *state);
int bambu_pages_unit(const bambu_state_t *state, int page);
int bambu_pages_find(const bambu_state_t *state, int unit_id);
bool bambu_pages_external(const bambu_state_t *state,int page);
int bambu_pages_ordinal(const bambu_state_t *state,int unit);
