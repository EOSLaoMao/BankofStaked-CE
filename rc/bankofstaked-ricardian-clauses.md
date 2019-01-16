# Ricardian Clauses for **BankofStaked**

## Preamble

EOS is a wonderful tool for the world to use in commercial applications; there are however some complications that may arise from its resource allocation and planning.

BankofStaked aims to help by providing an automated service for resource allocation within EOS in exchange for fixed costs over a predefined duration.

## Executive Summary

BankofStaked provides the workflow to arrange Creditor to temporarily delegate resources to Beneficiary in exchange for a Fee.

## Language and Interpretation

The operative language is English.

## Definitions

 * Beneficiary: the person who is recipient of delegation.
 * Chain: the chain with the ID `aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906`.
 * Creditor: the person who makes resources available for delegation.
 * Delegation: the assignment of resources from Creditor to Beneficiary.
 * Fee: tokens transfered into BankofStaked that exactly correspond to a fee schedule amount.
 * Fee Schedule: the list of services provided with their corresponding amount, which can be fetched from pricing table of BankofStaked smart contract.

## Roles

This smart contract defines the following roles:
1. Code: the account that manages this contract
1. Creditor: any account that grants Code delegate/undelegate permission on some of its token
1. Beneficiary: any account that pays fee in order to receive delegation

## Scope

This contract applies to Chain, with the ID as defined above. Each chain may have its own operative version of this contract. Each chain's contract is severable and independent.

## Acceptance

Any party making use or taking benefit from this contract is considered to agree to the terms set forth herein, as per their role as defined in Roles section.

## Role: Code

### Obligations

On a best effort basis, Code aims to:
1. maintain this contract's operational status.
1. grant delegation to beneficiary resources as per Fee Schedule.
1. return funds transfered to this contract which does not match Fee Schedule.
1. release delegation upon expiry.
1. to distribute 90% of the fee to Creditor whose token was invovled in a delegation.

### Rights

Code reserves the right to:
1. update the terms of Fee Schedule without notice nor justification.
1. revoke delegation to any beneficiary for violation of terms set forth herein.
1. decline service to any beneficiary without notice nor justification.

## Role: Creditor

### Obligations

1. Creditor agrees to refrain from revoking delegate/undelegate permission without submitting Code 3 days' prior notice, as per Communications Protocols defined herein.
   1. Failure to provide proper notice before revoking delegate/undelegate permission, resulting in interruption of service for Beneficiary, Creditor forfeits his right to proceeds from the fees.
1. All communications intended for Code are to be sent as per Communications Protocol prescribed below.

### Rights

1. Creditor has a right to 90% of proceeds from the fees accrued by a delegation at the time of release.

## Role: Beneficiary

### Obligations

1. To refrain from submitting an unreasonable amount of requests.
1. All communications intended for Code are to be sent as per Communications Protocol prescribed below.

### Rights

1. For duly submitted fees resulting in a delegation order, Beneficiary may have an expectation of maintenance of delegation.
1. Beneficiaries who suffer an interruption of delegation may claim either: (i) a refund, or (ii) credit toward a delegation of equal value.

## Communications Protocol

All notices under this contract intended for Code, must be sent by email to the following address: `bos@eoslaomao.com`
   
## Force Majeure

In any event outside of BankofStaked's ability to control or influence, BankofStaked is not liable for any failure to perform as a result of said event.

This may include--without limitation:
 * an interruption of service of the EOS Chain;
 * hacking/compromise of BankofStaked or Code account;
 * natural disater or act of God.
