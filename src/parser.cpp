// Author: Caden LeCluyse

#include "parser.h"

#include <algorithm>
#include <cctype>
#include <stack>
#include <stdexcept>
#include <string>

[[nodiscard]] bool Parser::is_operand(const char token) const noexcept { return (token == 'T' || token == 'F'); }

[[nodiscard]] bool Parser::is_operator(const char token) const noexcept {
    return (token == '&' || token == '|' || token == '@' || token == '$');
}

[[nodiscard]] bool Parser::is_not(const char token) const noexcept { return token == '!'; }

void Parser::check_leading(const std::string& infix_expression) {
    for (const auto i : infix_expression) {
        if (isspace(i)) {
            continue;
        } else if (is_operator(i)) {
            throw std::runtime_error("Expression begins with an operator\n\n!");
        } else if (i == ')') {
            throw std::runtime_error("Expression begins with closed parentheses!\n\n");
        } else {
            break;
        }
    }
}

void Parser::check_trailing(const std::string& infix_expression) {
    // I originally was just checking for the last value, but then I realized white space messed it all up
    for (auto itr = infix_expression.rbegin(); itr != infix_expression.rend(); ++itr) {
        if (isspace(*itr)) {
            continue;
        } else if (is_not(*itr)) {
            throw std::runtime_error("Expression ends with NOT!\n\n");
        } else if (is_operator(*itr)) {
            throw std::runtime_error("Expression ends with an operator!\n\n");
        } else if (*itr == '(') {
            throw std::runtime_error("Expression ends with open parentheses!\n\n");
        } else {
            break;
        }
    }
}

void Parser::error_checker() {
    // Checks for empty parentheses, consecutive operands and operators
    if (current_token == '(' && previous_token == ')') {
        throw std::runtime_error("Empty parentheses detected!\n\n");
    } else if (is_operator(current_token) && is_operator(previous_token)) {
        throw std::runtime_error("Two consecutive operators detected: " + std::string(1, current_token) + " and " +
                                 std::string(1, previous_token) + "\n\n");
    } else if (is_operand(current_token) && is_operand(previous_token)) {
        throw std::runtime_error("Two consecutive operands detected: " + std::string(1, current_token) + " and " +
                                 std::string(1, previous_token) + "\n\n");
    } else if (is_not(current_token) && (is_operator(previous_token) || previous_token == ')')) {
        throw std::runtime_error("NOT applied after value!\n\n");

        // Check if we are missing an operator
    } else if ((current_token == ')' && is_operand(previous_token)) ||
                (is_operand(current_token) && (previous_token == '(' || is_not(previous_token))) ||
               (current_token == ')' && previous_token == '(')) {
        throw std::runtime_error("Missing operator!\n\n");

        // Check if we are missing an operand
    } else if ((current_token == '(' && is_operator(previous_token)) ||
               (is_operator(current_token) && previous_token == ')')) {
        throw std::runtime_error("Missing an operand!\n\n");
    }
}

[[noreturn]] void Parser::throw_invalid_character_error(const char token) {
    if (isalnum(token)) {
        throw std::runtime_error("Expected T or F, received: " + std::string(1, token) + "\n\n");
    } else if (token == ']' || token == '[') {
        throw std::runtime_error("Invalid use of brackets detected! Just use parentheses please.\n\n");
    }
    throw std::runtime_error("Expected &, |, !, @, $, received: " + std::string(1, token) + "\n\n");
}

[[nodiscard]] std::string Parser::create_prefix_expression(const std::string& infix_expression) {
    std::string prefix_expression;
    current_token = '\0';
    previous_token = '\0';
    int open_parentheses = 0;
    int closed_parentheses = 0;

    // Check if the string is only white space
    if (std::ranges::all_of(infix_expression, isspace)) {
        throw std::runtime_error("Expression contains only spaces!\n\n");
    }
    check_leading(infix_expression);
    check_trailing(infix_expression);

    // Traverse the string in reverse
    for (auto itr = infix_expression.rbegin(); itr != infix_expression.rend(); ++itr) {
        // Ignore white space
        if (isspace(*itr)) {
            continue;
        }

        // Grab the current token
        // If the token is an operand, simply add it to the string
        // if the token is an operator or a closing parentheses, add it to the stack
        // If the token is an open parentheses, pop from the stack and add to the string until a closing parentheses is
        // found
        current_token = *itr;

        // Check for various errors
        error_checker();

        if (is_operand(current_token)) {
            prefix_expression.push_back(current_token);
        } else if (is_not(current_token) || is_operator(current_token) || current_token == ')') {
            if (current_token == ')') {
                closed_parentheses++;
            }
            operator_stack.push(current_token);
        } else if (current_token == '(') {
            open_parentheses++;

            while (!operator_stack.empty() && operator_stack.top() != ')') {
                prefix_expression.push_back(operator_stack.top());
                operator_stack.pop();
            }

            // Pop the closing parentheses off the stack
            if (!operator_stack.empty()) {
                operator_stack.pop();
            } else {
                throw std::runtime_error("Missing closing parentheses!\n\n");
            }
        } else {
            throw_invalid_character_error(current_token);
        }
        previous_token = current_token;
    }

    if (open_parentheses < closed_parentheses) {
        throw std::runtime_error("Missing open parentheses!\n\n");
    }

    while (!operator_stack.empty()) {
        prefix_expression.push_back(operator_stack.top());
        operator_stack.pop();
    }
    std::ranges::reverse(prefix_expression);
    return prefix_expression;
}
