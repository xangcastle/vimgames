#include "levels.h"

// --- Level Definitions ---

// Sector 0, Level 1: Basic movement and deletion
Level get_level_0_1() {
    Level level;
    level.initial_buffer = {
        "This is an eror line.",
        "Fix the typo hear.",
        "And another won here."
    };
    level.target_buffer = {
        "This is an error line.",
        "Fix the typo here.",
        "And another one here."
    };
    return level;
}

// Sector 1, Level 1: Word movements
Level get_level_1_1() {
    Level level;
    level.initial_buffer = {
        "delete-this-word and this-one-too",
        "and this-one-also"
    };
    level.target_buffer = {
        "and",
        "also"
    };
    return level;
}

// Sector 2, Level 1: Visual mode, copy/paste
Level get_level_2_1() {
    Level level;
    level.initial_buffer = {
        "line to be copied",
        "paste here -> ",
    };
    level.target_buffer = {
        "line to be copied",
        "paste here -> line to be copied",
    };
    return level;
}

// Sector 3, Level 1: Search
Level get_level_3_1() {
    Level level;
    level.initial_buffer = {
        "find the secret word",
        "the secret word is 'synergy'",
        "jump to synergy and delete it"
    };
    level.target_buffer = {
        "find the secret word",
        "the secret word is ''",
        "jump to synergy and delete it"
    };
    return level;
}

// Sector 4, Level 1: Replace
Level get_level_4_1() {
    Level level;
    level.initial_buffer = {
        "Rreplace this line with correct characters",
        "and tis one tooo"
    };
    level.target_buffer = {
        "Replace this line with correct characters",
        "and this one too"
    };
        return level;
    }
    
    // Sector 0 Boss: "The Rust Gate" - typos to fix
    Level get_level_0_boss() {
        Level level;
        level.initial_buffer = {
            "This is a ruset gate. It iz full of erors.",
            "Yoou musrt fix dem charracter byy characer.",
            "Manny, manny misstakes in teh syystem.",
            "Cleen it upp, hackerr.",
            "All typoss shuld be corrrrected."
        };
        level.target_buffer = {
            "This is a rust gate. It is full of errors.",
            "You must fix them character by character.",
            "Many, many mistakes in the system.",
            "Clean it up, hacker.",
            "All typos should be corrected."
        };
        return level;
    }
    
    // --- Level Management ---
    

static std::vector<Level> all_levels = {
    get_level_0_1(),
    get_level_0_boss(), // Add the boss level here
    get_level_1_1(),
    get_level_2_1(),
    get_level_3_1(),
    get_level_4_1()
};

Level get_level(int level_index) {
    if (level_index >= 0 && level_index < all_levels.size()) {
        return all_levels[level_index];
    }
    // Return a default/error level or handle error appropriately
    return get_level_0_1(); 
}

int get_total_levels() {
    return all_levels.size();
}
