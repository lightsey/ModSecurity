/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include "modsecurity/rule_message.h"

#include "modsecurity/rules_set.h"
#include "modsecurity/modsecurity.h"
#include "modsecurity/transaction.h"
#include "src/utils/string.h"
#include "src/rule_with_actions.h"


namespace modsecurity {

   /**
     *
     * FIXME: RuleMessage is currently too big, doing a lot of
     * unnecessary data duplication. Needs to be shrink down.
     *
     */
RuleMessage::RuleMessage(RuleWithActions *rule, Transaction *trans) :
    m_accuracy(rule->getAccuracy()),
    m_clientIpAddress(trans->m_clientIpAddress),
    m_data(""),
    m_id(trans->m_id),
    m_isDisruptive(false),
    m_match(""),
    m_maturity(rule->getMaturity()),
    m_message(""),
    m_noAuditLog(false),
    m_phase(rule->getPhase() - 1),
    m_reference(""),
    m_rev(*rule->getRevision()),
    m_rule(rule),
    m_ruleFile(rule->getFileName()),
    m_ruleId(rule->getId()),
    m_ruleLine(rule->getLineNumber()),
    m_saveMessage(true),
    m_serverIpAddress(trans->m_serverIpAddress),
    m_severity(0),
    m_uriNoQueryStringDecoded(trans->m_uri_no_query_string_decoded),
    m_ver(*rule->getVersion())
    { }

RuleMessage::RuleMessage(RuleMessage *rule) :
    m_accuracy(rule->m_accuracy),
    m_clientIpAddress(rule->m_clientIpAddress),
    m_data(rule->m_data),
    m_id(rule->m_id),
    m_isDisruptive(rule->m_isDisruptive),
    m_match(rule->m_match),
    m_maturity(rule->m_maturity),
    m_message(rule->m_message),
    m_noAuditLog(rule->m_noAuditLog),
    m_phase(rule->m_phase),
    m_reference(rule->m_reference),
    m_rev(rule->m_rev),
    m_rule(rule->m_rule),
    m_ruleFile(rule->m_ruleFile),
    m_ruleId(rule->m_ruleId),
    m_ruleLine(rule->m_ruleLine),
    m_saveMessage(rule->m_saveMessage),
    m_serverIpAddress(rule->m_serverIpAddress),
    m_severity(rule->m_severity),
    m_uriNoQueryStringDecoded(rule->m_uriNoQueryStringDecoded),
    m_ver(rule->m_ver)
    { }

std::string RuleMessage::_details(const RuleMessage *rm) {
    std::string msg;

    msg.append(" [file \"" + std::string(*rm->m_ruleFile.get()) + "\"]");
    msg.append(" [line \"" + std::to_string(rm->m_ruleLine) + "\"]");
    msg.append(" [id \"" + std::to_string(rm->m_ruleId) + "\"]");
    msg.append(" [rev \"" + rm->m_rev + "\"]");
    msg.append(" [msg \"" + rm->m_message + "\"]");
    msg.append(" [data \"" + rm->m_data + "\"]");
    msg.append(" [severity \"" +
        std::to_string(rm->m_severity) + "\"]");
    msg.append(" [ver \"" + rm->m_ver + "\"]");
    msg.append(" [maturity \"" + std::to_string(rm->m_maturity) + "\"]");
    msg.append(" [accuracy \"" + std::to_string(rm->m_accuracy) + "\"]");
    for (auto &a : rm->m_tags) {
        msg.append(" [tag \"" + a + "\"]");
    }
    msg.append(" [hostname \"" + *rm->m_serverIpAddress.get() \
        + "\"]");
    msg.append(" [uri \"" + *rm->m_uriNoQueryStringDecoded.get() + "\"]");
    msg.append(" [unique_id \"" + *rm->m_id + "\"]");
    msg.append(" [ref \"" + rm->m_reference + "\"]");

    return msg;
}


std::string RuleMessage::_errorLogTail(const RuleMessage *rm) {
    std::string msg;

    msg.append("[hostname \"" + *rm->m_serverIpAddress.get() + "\"]");
    msg.append(" [uri \"" + *rm->m_uriNoQueryStringDecoded.get() + "\"]");
    msg.append(" [unique_id \"" + *rm->m_id + "\"]");

    return msg;
}


std::string RuleMessage::log(const RuleMessage *rm, int props, int code) {
    std::string msg("");
    msg.reserve(2048);

    if (props & ClientLogMessageInfo) {
        msg.append("[client " + std::string(*rm->m_clientIpAddress.get()) + "] ");
    }

    if (rm->m_isDisruptive) {
        msg.append("ModSecurity: Access denied with code ");
        if (code == -1) {
            msg.append("%d");
        } else {
            msg.append(std::to_string(code));
        }
        msg.append(" (phase ");
        msg.append(std::to_string(rm->m_rule->getPhase() - 1) + "). ");
    } else {
        msg.append("ModSecurity: Warning. ");
    }

    msg.append(rm->m_match);
    msg.append(_details(rm));

    if (props & ErrorLogTailLogMessageInfo) {
        msg.append(" " + _errorLogTail(rm));
    }

    return modsecurity::utils::string::toHexIfNeeded(msg);
}


}  // namespace modsecurity
