#include <algorithm>
#include <bits/stdc++.h>
#include <string>
#include "error.hpp"
#include "genrex.hpp"

/// @brief [DEBUG] display a regex rule
/// @param units the regex rule
void show_regex(std::vector<genrex::RegUnit*> units)
{
    for (size_t i = 0; i < units.size(); i++) {
        genrex::RegUnit* current = units[i];
        for (size_t j = 0; j < current->chars.size(); j++) {
            printf("%c", current->chars[j]);
        }
        printf(" from %d to %d", current->min, current->max);
    }
    printf("\n");
}

/// @brief set a range of char in unit
/// @param _regunit the regex unit to modify
/// @param from the first charactere of range
/// @param to the last charactere of range (include)
void set_chars_range(genrex::RegUnit* _regunit, char from, char to) {
    if (from > to)
        handle_error(ERR_FORMAT);
    for (char value = from; value <= to; value++)
        _regunit->chars.push_back(value);
}

/// @brief set a size range in unit
/// @param _regunit the regex unit to modify
/// @param min minimum of range
/// @param max maximum of range
void set_range(genrex::RegUnit* _regunit, int min, int max) {
    if (min > max) 
        handle_error(ERR_FORMAT);
    _regunit->min = min;
    _regunit->max = max;
}

/// @brief [BETA] remove sepecifics chars in unit
/// @param _regunit the regex unit to modify
/// @param to_removed a vector of characteres to remove
void remove_chars(genrex::RegUnit* _regunit, std::vector<char> to_removed) {
    _regunit->chars.erase(std::remove_if(_regunit->chars.begin(), _regunit->chars.end(),
        [&to_removed](char elem) {
            return std::find(to_removed.begin(), to_removed.end(), elem) != to_removed.end();
        }), _regunit->chars.end());
}

/// @brief interprete special char (\<special>)
/// @param _regunit the regex unit to modify
/// @param special the special charactere to interprete
void special_chars(genrex::RegUnit* _regunit, char special) {
    printf("%c\n", special);
    switch (special)
    {
    case 'd':
        set_chars_range(_regunit, '0', '9');
        break;
    case 'w':
        set_chars_range(_regunit, 'a', 'z');
        set_chars_range(_regunit, 'A', 'Z');
        set_chars_range(_regunit, '0', '9');
        break;
    case 's':
        _regunit->chars.push_back('\t');
        _regunit->chars.push_back('\n');
        _regunit->chars.push_back('\r');
        _regunit->chars.push_back('\x0B');
        _regunit->chars.push_back('\f');
        break;
    case 'D': {
        std::vector<char> to_remove;
        for (char value = '0'; value <= '9'; value++) {
            to_remove.push_back(value);
        }
        set_chars_range(_regunit, 33, 126);
        remove_chars(_regunit, to_remove);
    }
        break;
    case 'W': {
        std::vector<char> to_remove;
        char value;
        for (value = '0'; value <= '9'; value++) {
            to_remove.push_back(value);
        }
        for (value = 'a'; value <= 'z'; value++) {
            to_remove.push_back(value);
        }
        for (value = 'A'; value <= 'Z'; value++) {
            to_remove.push_back(value);
        }
        set_chars_range(_regunit, 33, 126);
        remove_chars(_regunit, to_remove);
    }
        break;
    case 'S': {
        std::vector<char> to_remove = { '\n', '\t', '\f', '\r', '\x0B' };
        set_chars_range(_regunit, 33, 126);
        remove_chars(_regunit, to_remove);
    }
        break;
    case '\\':
    case '[':
    case '(':
    case '{':
        _regunit->chars.push_back(special); 
        break;
    default:
        handle_error(ERR_FORMAT);
        break;
    }
    std::sort(_regunit->chars.begin(), _regunit->chars.end());
}

/// @brief compile a pattern into a vector of regunit
/// @param pattern the pattern to compile
/// @return the compiled regex
std::vector<genrex::RegUnit*> genrex::compile(char* pattern)
{
    size_t pattern_size = strlen(pattern);
    std::vector<genrex::RegUnit*> list;
    genrex::RegUnit* unit = new RegUnit();
    bool init = false;
    for (size_t index = 0; index < pattern_size; index++) {
        char current = pattern[index];
        switch (current)
        {
        case '[':
            if (init) {
                list.push_back(unit);
                unit = new RegUnit();
            } else {
                init = true;
            }
            for (++index; pattern[index] != ']'; index++) {
                current = pattern[index];
                if ((index+1) >= pattern_size) 
                    handle_error(PREMATURE_END);
                if (pattern[index+1] != '-') {
                    unit->chars.push_back(current);
                } else {
                    if ((index+2) >= pattern_size)
                        handle_error(PREMATURE_END);
                    char from = current, to = pattern[index+2];
                    set_chars_range(unit, from, to);
                    index += 2;
                }
            }
            std::sort(unit->chars.begin(), unit->chars.end());
            break;
        case '{': {
            std::string buffer;
            int min = 0, max = 0;
            bool have_comma = false;
            for (++index; pattern[index] != '}'; index++) {
                current = pattern[index];
                if (current == ',') {
                    if (have_comma)
                        handle_error(ERR_FORMAT);
                    have_comma = true;
                    min = atoi(buffer.c_str());
                    if ((index+1) >= pattern_size) 
                        handle_error(PREMATURE_END);
                    if (pattern[index+1] == '}') {
                        max = UNIT_MAXLEN;
                        continue;
                    }
                } else if (current >= '0' && current <= '9') {
                    buffer += current;
                } else {
                    handle_error(ERR_FORMAT);
                }
            }
            if (!have_comma) {
                min = atoi(buffer.c_str());
                max = min;
            } else if (buffer.size() != 0) {
                max = atoi(buffer.c_str());
            }
            set_range(unit, min, max);
            break;
        }
        case '(':
            // Not implemented
            handle_error(NOT_IMPLEMENTED);
            break;
        case '\\':
            if (index+1 >= pattern_size)
                handle_error(PREMATURE_END);
            if (init) {
                list.push_back(unit);
                unit = new RegUnit();
            } else {
                init = true;
            }
            special_chars(unit, pattern[index+1]);
            index++;
            break;
        case '+':
            set_range(unit, 1, UNIT_MAXLEN);
            break;
        case '*':
            set_range(unit, 0, UNIT_MAXLEN);
            break;
        case '?':
            set_range(unit, 0, 1);
            break;
        case '.':
            if (init) {
                list.push_back(unit);
                unit = new RegUnit();
                unit->chars = std::vector<char>();
            } else {
                init = true;
            }
            set_chars_range(unit, 33, 126);
            break;
        default:
            if (init) {
                list.push_back(unit);
                unit = new RegUnit();
            } else {
                init = true;
            }
            unit->chars.push_back(current);
            break;
        }
    }
    if (init)
        list.push_back(unit);
    return list;
}

/// @brief free space of compiled regex
/// @param units 
void genrex::free_regunits(std::vector<genrex::RegUnit*> units) {
    for (long unsigned int i = 0; i < units.size(); i++)
        delete units[i];
    units.clear();
}

/// @brief build a regex from pattern and random objetc
/// @param pattern the used pattern
/// @param random the random to use
genrex::GenRex::GenRex(char* pattern, Random random) :
    pattern(genrex::compile(pattern)), random(random)
{}

/// @brief destructor of regex
genrex::GenRex::~GenRex()
{
    free_regunits(this->pattern);
}

/// @brief the function to generate a string using the regex
/// @return the generated string
std::string genrex::GenRex::generate_string()
{
    std::string buffer = "";
    for (size_t index = 0; index < this->pattern.size(); index++) {
        genrex::RegUnit *current = this->pattern[index];    
        int len = current->min == current->max ? current->min : this->random.next() % (current->max - current->min) + current->min;
        for (int i = 0; i < len; i++)
        {
            int char_id = this->random.next() % current->chars.size();
            buffer += current->chars[char_id];
        }
    }
    return buffer;
}

/// @brief build a default regunit
/// `empty`{1}
genrex::RegUnit::RegUnit()
    : chars(), min(1), max(1)
{}

/// @brief destructor of regunit
genrex::RegUnit::~RegUnit()
{
    chars.clear();
    min = 0;
    max = 0;
}
