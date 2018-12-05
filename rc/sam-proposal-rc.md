# Ricardian Clauses for **BankofStaked**

## Preamble

The EOS Mainnet is a wonderful tool for the world to use in commercial applications; there are however some complications that may arise from its resource allocation and planning.

BankofStaked aims to help by providing an automated service for resource allocation within the EOS Mainnet in exchange for fixed costs over a predefined duration.

## Executive Summary

BankofStaked provides the workflow to arrange Creditor to temporarily delegate staked resources to Beneficiary in exchange for a Fee.

## Language and Interpretation

The operative language is English.

## Definitions

 * Beneficiary: the person who is recipient of delegation.
 * Creditor: the person who makes stake available for delegation.
 * Delegation: the assignment of resources from Creditor to Beneficiary.
 * Fee: tokens transfered into BankofStaked that exactly correspond to a fee schedule amount.
 * Fee Schedule: the list of services provided with their corresponding amount.
 * Stake: the token which a Creditor makes available for delegation.

## Roles

This smart contract defines the following roles:
1. Code: the account that manages this contract
1. Creditor: any account that makes stake available for delegation
1. Beneficiary: any account that pays fee in order to receive delegation

## Acceptance

Any party making use or taking benefit from this contract is considered to agree to the terms set forth herein, as per their role as defined in Roles section.

## Role: Code

### Obligations

1. On a best effort basis, Code is responsible for maintaining this contract's operational status.
1. On a best effort basis, Code aims to grant delegation to beneficiary resources as per Fee Schedule.
1. On a best effort basis, Code aims to return funds transfered to this contract which does not match Fee Schedule.
1. On a best effort basis, Code aims to release delegation upon expiry.

### Rights

1. Reserves the right to update the terms of Fee Schedule without notice nor justification.
1. Code reserves the right to revoke delegation to any beneficiary for violation of terms set forth herein.

## Role: Creditor

### Obligations

Creditor pledges to [...]

### Rights

Creditor may reserve the right to [...]

## Role: Beneficiary

### Obligations

1. Beneficiary must not submit unreasonable amounts of orders.

## Force Majeure

In any event outside of BankofStaked's ability to control or influence, BankofStaked is not liable for any failure to perform as a result of said event. This may include--without limitation--an interruption of service of the EOS Main Net or the Internet.

## Fee Schedule

| Package | Amount | RAM | Bandwidth | Duration |
|---|---|---|---|---|
| Free Plan | 0 | 0.8 | 0.2 | 8 hours |
| Micro | 0.2 | 55 | 5 | 7 days |
| Mini | 0.5 | 165 | 15 | 7 days |
| Short | 1.0 | 330 | 20 | 7 days |
| Tall | 2.0 | 700 | 50 | 7 days |

Note: all amounts set in Fee Schedule are denominated in EOS token.
