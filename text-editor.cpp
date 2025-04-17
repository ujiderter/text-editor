#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cctype>
#include <regex>

enum class CommandType {
    SUBSTITUTION,
    DELETION,
    TRANSLITERATION,
    INSERTION
};

struct Address {
    bool is_specified = false;
    bool is_range = false;
    int start = 0;
    int end = 0;
};

struct Command {
    CommandType type;
    Address addr;
    std::string pattern;
    std::string replacement;
    bool global = false;
};

std::vector<Command> parse_commands(const std::vector<std::string>& command_lines, int& valid_command_count);
bool parse_command(const std::string& command_line, Command& cmd);
bool parse_address(const std::string& addr_str, Address& addr);
std::unique_ptr<std::string> process_line(const std::string& line, const std::vector<Command>& commands, int line_num);
std::string apply_substitution(const std::string& line, const Command& cmd);
std::string apply_transliteration(const std::string& line, const Command& cmd);
std::string apply_insertion(const std::string& line, const Command& cmd);
bool should_delete(const std::string& line, const Command& cmd);
bool is_address_match(int line_num, const Address& addr);

int main() {
    int M, N;

    if (!(std::cin >> M) || M <= 0 || M > 1000000) {
        std::cout << "Error: Invalid number of commands." << std::endl;
        return 1;
    }

    std::cin.ignore();

    std::vector<std::string> command_lines;
    command_lines.reserve(M);

    for (int i = 0; i < M; i++) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "Error: Cannot read command " << i + 1 << "." << std::endl;
            return 1;
        }
        command_lines.push_back(line);
    }

    int valid_command_count = 0;
    std::vector<Command> commands = parse_commands(command_lines, &valid_command_count);

    if (valid_command_count == 0) {
        return 1;
    }

    if (!(std::cin >> N) || N <= 0 || N > 100000) {
        std::cout << "Error: Invalid number of text lines." << std::endl;
        return 1;
    }

    std::cin.ignore();

    for (int i = 1; i <= N; i++) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        auto processed_line = process_line(line, commands, i);

        if (processed_line) {
            std::cout << *processed_line << std::endl;
        }
    }

    return 0;
}

std::vector<Command> parse_commands(const std::vector<std::string>& command_lines, int& valid_command_count) {
    std::vector<Command> commands;
    commands.reserve(command_lines.size());
    valid_command_count = 0;

    for (const auto& line : command_lines) {
        Command cmd;
        if (parse_command(line, cmd)) {
            commands.push_back(cmd);
            (*valid_command_count)++;
        }
    }

    return commands;
}

bool parse_command(const std::string& command_line, Command& cmd) {
    std::string cmd_str = command_line;
    std::string original_cmd = command_line;

    cmd.addr.is_specified = false;
    cmd.addr.is_range = false;
    cmd.global = false;

    std::string cmd_part;
    std::string addr_str;

    size_t cmd_char_pos = std::string::npos;
    for (size_t i = 0; i < cmd_str.length(); i++) {
        char c = cmd_str[i];
        if ((c == 's' || c == 'd' || c == 'y' || c == 'i') && 
            (i == 0 || cmd_str[i-1] == ' ')) {
            cmd_char_pos = i;
            break;
        }
    }

    bool has_address = false;
    if (cmd_char_pos != std::string::npos && cmd_char_pos > 0) {
        addr_str = cmd_str.substr(0, cmd_char_pos);
        cmd_part = cmd_str.substr(cmd_char_pos);
        has_address = true;
        
        // Trim whitespace from address
        addr_str.erase(0, addr_str.find_first_not_of(" "));
    } else if (cmd_char_pos != std::string::npos) {
        cmd_part = cmd_str.substr(cmd_char_pos);
    } else {
        std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
        return false;
    }

    if (has_address) {
        if (!parse_address(addr_str, cmd.addr)) {
            std::cout << "Error in command \"" << original_cmd << "\": Invalid line number." << std::endl;
            return false;
        }
    }

    if (cmd_part[0] == 's') {
        cmd.type = CommandType::SUBSTITUTION;
    } else if (cmd_part[0] == 'd') {
        cmd.type = CommandType::DELETION;
    } else if (cmd_part[0] == 'y') {
        cmd.type = CommandType::TRANSLITERATION;
    } else if (cmd_part[0] == 'i') {
        cmd.type = CommandType::INSERTION;
    } else {
        std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
        return false;
    }

    char delimiter = '/';
    size_t pos = cmd_part.find(delimiter);
    
    if (pos != 1) {
        std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
        return false;
    }

    size_t pattern_start = pos + 1;
    size_t pattern_end = cmd_part.find(delimiter, pattern_start);
    
    if (pattern_end == std::string::npos) {
        std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
        return false;
    }

    cmd.pattern = cmd_part.substr(pattern_start, pattern_end - pattern_start);
    
    if (cmd.pattern.empty()) {
        std::cout << "Error in command \"" << original_cmd << "\": Parameter error." << std::endl;
        return false;
    }

    if (cmd.type == CommandType::SUBSTITUTION || cmd.type == CommandType::TRANSLITERATION || cmd.type == CommandType::INSERTION) {
        size_t replacement_start = pattern_end + 1;
        size_t replacement_end = cmd_part.find(delimiter, replacement_start);
        
        if (replacement_end == std::string::npos) {
            std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
            return false;
        }

        cmd.replacement = cmd_part.substr(replacement_start, replacement_end - replacement_start);
        
        if ((cmd.type == CommandType::SUBSTITUTION || cmd.type == CommandType::INSERTION) && cmd.replacement.empty()) {
            std::cout << "Error in command \"" << original_cmd << "\": Parameter error." << std::endl;
            return false;
        }

        if (cmd.type == CommandType::TRANSLITERATION && cmd.pattern.length() != cmd.replacement.length()) {
            std::cout << "Error in command \"" << original_cmd << "\": Parameter error." << std::endl;
            return false;
        }
        
        std::string remaining = cmd_part.substr(replacement_end + 1);
        if (cmd.type == CommandType::SUBSTITUTION && remaining == "g") {
            cmd.global = true;
        } else if (!remaining.empty()) {
            std::cout << "Error in command \"" << original_cmd << "\": Command syntax error." << std::endl;
            return false;
        }
    }

    return true;
}

bool parse_address(const std::string& addr_str, Address& addr) {
    addr.is_specified = true;

    size_t comma_pos = addr_str.find(',');
    if (comma_pos != std::string::npos) {
        addr.is_range = true;

        std::string start_str = addr_str.substr(0, comma_pos);
        std::string end_str = addr_str.substr(comma_pos + 1);

        // Trim whitespace
        start_str.erase(0, start_str.find_first_not_of(" "));
        start_str.erase(start_str.find_last_not_of(" ") + 1);
        end_str.erase(0, end_str.find_first_not_of(" "));
        end_str.erase(end_str.find_last_not_of(" ") + 1);

        // Check if strings contain only digits
        if (start_str.empty() || end_str.empty() ||
            !std::all_of(start_str.begin(), start_str.end(), ::isdigit) ||
            !std::all_of(end_str.begin(), end_str.end(), ::isdigit)) {
            return false;
        }

        addr.start = std::stoi(start_str);
        addr.end = std::stoi(end_str);

        if (addr.start <= 0 || addr.end <= 0 || addr.start > addr.end) {
            return false;
        }
    } else {
        addr.is_range = false;

        std::string num_str = addr_str;
        // Trim whitespace
        num_str.erase(0, num_str.find_first_not_of(" "));
        num_str.erase(num_str.find_last_not_of(" ") + 1);

        // Check if string contains only digits
        if (num_str.empty() || !std::all_of(num_str.begin(), num_str.end(), ::isdigit)) {
            return false;
        }

        addr.start = std::stoi(num_str);

        if (addr.start <= 0) {
            return false;
        }
    }

    return true;
}

std::unique_ptr<std::string> process_line(const std::string& line, const std::vector<Command>& commands, int line_num) {
    std::string current = line;

    for (const auto& cmd : commands) {
        // Check if command applies to this line
        if (!is_address_match(line_num, cmd.addr)) {
            continue;
        }

        switch (cmd.type) {
        case CommandType::SUBSTITUTION: {
            current = apply_substitution(current, cmd);
            break;
        }
        case CommandType::DELETION: {
            if (should_delete(current, cmd)) {
                return nullptr;
            }
            break;
        }
        case CommandType::TRANSLITERATION: {
            current = apply_transliteration(current, cmd);
            break;
        }
        case CommandType::INSERTION: {
            current = apply_insertion(current, cmd);
            break;
        }
        }
    }

    return std::make_unique<std::string>(current);
}

std::string apply_substitution(const std::string& line, const Command& cmd) {
    std::string result = line;
    std::string pattern = cmd.pattern;
    std::string replacement = cmd.replacement;
    
    size_t pos = 0;
    while ((pos = result.find(pattern, pos)) != std::string::npos) {
        result.replace(pos, pattern.length(), replacement);
        pos += replacement.length();
        
        if (!cmd.global) {
            break;
        }
    }
    
    return result;
}

std::string apply_transliteration(const std::string& line, const Command& cmd) {
    std::string result = line;
    
    for (size_t i = 0; i < result.length(); i++) {
        size_t find_pos = cmd.pattern.find(result[i]);
        if (find_pos != std::string::npos) {
            result[i] = cmd.replacement[find_pos];
        }
    }
    
    return result;
}

std::string apply_insertion(const std::string& line, const Command& cmd) {
    std::string result = line;
    size_t pos = result.find(cmd.pattern);
    
    if (pos != std::string::npos) {
        result.insert(pos, cmd.replacement);
    }
    
    return result;
}

bool should_delete(const std::string& line, const Command& cmd) {
    return line.find(cmd.pattern) != std::string::npos;
}

bool is_address_match(int line_num, const Address& addr) {
    if (!addr.is_specified) {
        return true;
    }

    if (addr.is_range) {
        return (line_num >= addr.start && line_num <= addr.end);
    } else {
        return (line_num == addr.start);
    }
}
