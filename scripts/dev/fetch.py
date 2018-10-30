import pprint
from eosapi import Client


c = Client(nodes=['https://api.eoslaomao.com'])
def fetch_creditors():
    paid_accounts = []
    free_accounts = []
    r = c.get_table_rows(**{"code": "bankofstaked", "scope": "921459758687", "table": "creditor", "json": True, "limit": 100, "upper_bound": None, "lower_bound": None, "table_key": "account_name"})

    for a in r["rows"]:
        #print(a)
        if a["for_free"] == 1:
            free_accounts.append(a)
        else:
            paid_accounts.append(a)
    return free_accounts, paid_accounts


def get_amount(asset):
    amount = float(asset.split(" ")[0])
    return amount

def get_account(a, free=True):
    r = c.get_account(a["account"])
    ram_quota = r["ram_quota"]
    liquid_balance = get_amount(r["core_liquid_balance"])
    balance = liquid_balance + get_amount(a["cpu_staked"]) + get_amount(a["net_staked"])
    if r["self_delegated_bandwidth"]:
        self_delband = r["self_delegated_bandwidth"]
        balance += get_amount(self_delband["cpu_weight"]);
        balance += get_amount(self_delband["net_weight"]);
    if r["refund_request"]:
        refundings.append(r["refund_request"])
        balance += get_amount(r["refund_request"]["cpu_amount"]);
        balance += get_amount(r["refund_request"]["net_amount"]);
    if free:
        ram_required = balance * 0.16
    else:
        ram_required = balance * 0.12 / 30.
    print(r["account_name"], liquid_balance)
    print(r["account_name"], "%.4f EOS" % balance, "%.2f" % (ram_quota/1024.), "%.2f" % ram_required, ram_quota/1024. > ram_required)


if __name__ == "__main__":
    refundings = []
    free_accounts, paid_accounts = fetch_creditors()
    print("=================FREE ACCOUNTS==================")
    for account in free_accounts:
        get_account(account)
    print("================================================\n\n")

    print("=================PAID ACCOUNTS==================")
    for account in paid_accounts:
        get_account(account, False)
    print("================================================")

    for r in refundings:
        print(r)
