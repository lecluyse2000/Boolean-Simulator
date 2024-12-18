// Author: Caden LeCluyse

#include "ast.h"

#include <cctype>
#include <string_view>

#include "../types/types.hpp"
#include "node.h"

std::unique_ptr<Node> AST::build_ast() noexcept {
    const char current_token = m_prefix_expression[m_index++];

    if (Types::isoperand(current_token)) {
        return std::make_unique<BoolNode>(current_token);
    }

    std::unique_ptr<Node> node;
    if (Types::isnot(current_token)) {
        node = std::make_unique<UnaryNode>(current_token);
        node->m_left_child = build_ast();
    } else {
        node = std::make_unique<OperationNode>(current_token);
        node->m_left_child = build_ast();
        node->m_right_child = build_ast();
    }

    return node;
}

AST::AST(const std::string_view expression) noexcept : m_prefix_expression(expression), m_index(0),  m_root(build_ast()) {}

[[nodiscard]] bool AST::evaluate() const { return m_root->evaluate(); }
