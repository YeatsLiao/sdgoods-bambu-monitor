/* SPDX-License-Identifier: MIT
 * Adapted from ai-passport-bambu-monitor; see docs/architecture/upstream.md.
 */
#include "bambu_pages.h"
static bool ht(const bambu_ams_unit_t *u) { return u->kind == BAMBU_AMS_HT; }
int bambu_pages_count(const bambu_state_t *st) {
    bool has_ht=false;
    for(int i=0;i<st->unit_count;i++) if(ht(&st->units[i])) has_ht=true;
    return 2+st->unit_count+(st->external_present && !has_ht ? 1:0);
}
int bambu_pages_unit(const bambu_state_t *st,int page) {
    if(page<=0 || page>=bambu_pages_count(st)) return BAMBU_PAGE_OVERVIEW;
    if(page==bambu_pages_count(st)-1) return BAMBU_PAGE_DETAILS;
    int n=1;
    for(int group=0;group<2;group++)
        for(int i=0;i<st->unit_count;i++)
            if(ht(&st->units[i])==(group==1) && n++==page) return i;
    return BAMBU_PAGE_EXTERNAL;
}
bool bambu_pages_external(const bambu_state_t *st,int page) {
    if(!st->external_present) return false;
    int unit=bambu_pages_unit(st,page);
    if(unit==BAMBU_PAGE_EXTERNAL) return true;
    if(unit<0 || !ht(&st->units[unit])) return false;
    for(int i=0;i<unit;i++) if(ht(&st->units[i])) return false;
    return true;
}
int bambu_pages_ordinal(const bambu_state_t *st,int unit) {
    int ordinal=0;
    for(int i=0;i<=unit && i<st->unit_count;i++)
        if(st->units[i].kind==st->units[unit].kind) ordinal++;
    return ordinal;
}
int bambu_pages_find(const bambu_state_t *st,int unit_id) {
    for(int page=1;page<bambu_pages_count(st)-1;page++) {
        int u=bambu_pages_unit(st,page);
        if(unit_id==254 && bambu_pages_external(st,page)) return page;
        if(u>=0 && st->units[u].id==unit_id) return page;
    }
    return 0;
}
