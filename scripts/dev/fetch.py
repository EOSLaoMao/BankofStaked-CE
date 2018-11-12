import pprint
import time
from eosapi import Client

c = Client(nodes=['https://api.eoslaomao.com'])

def check_order(lower_bound=1):
    expired_orders = []
    now = time.time()
    new_lower_bound = lower_bound
    r = c.get_table_rows(**{"code": "bankofstaked", "scope": "921459758687", "table": "order", "json": True, "limit": 10000, "upper_bound": None, "lower_bound": lower_bound, "table_key": "id"})
    more = r["more"]
    count = 0
    free_count = 0
    for line in r["rows"]:
        if line["expire_at"] < now:
            expired_orders.append(line)
            count+=1
            print(line)
            if line["is_free"]:
                #print("free id:", line["id"])
                free_count += 1
        if line["id"] > lower_bound:
            new_lower_bound = line["id"]
    print("expired orders: %d" % count)
    print("expired free orders: %d" % free_count)
    return more, new_lower_bound, expired_orders



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
    #print(r["account_name"], liquid_balance)
    row = " | ".join([r["account_name"], str("%.4f EOS" % balance), str("%.2f" % (ram_quota/1024.)), str("%.2f" % ram_required), "✅" if (ram_quota/1024. > ram_required) else "❌"])
    row = "| %s |" % row
    print(row)


if __name__ == "__main__":
    '''
    bps = set()
    def get_name(d):
        """ Return the value of a key in a dictionary. """
        return d["expire_at"]

    expired = []
    more, lower_bound, expired_orders = check_order()
    expired.extend(expired_orders)
    while more:
        more, lower_bound, expired_orders = check_order(lower_bound=lower_bound)
        expired.extend(expired_orders)

    expired.sort(key=get_name)
    ids = []
    for e in expired:
        bps.add(e["creditor"]);
        if(e["creditor"] == 'stakingfundh'):
            print(e)
            ids.append(e["id"])
        #print(e["id"], (e["expire_at"]-time.time())/3600, (e["expire_at"]-e["created_at"])/3600.)
    print(bps)
    ids.reverse()
    print(ids[:100])
    '''
    refundings = []
    free_accounts, paid_accounts = fetch_creditors()
    print("=================FREE ACCOUNTS==================")
    print("| Account | Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |")
    print("| ------- | ------- | --------- | ----------- | ----------- |")
    for account in free_accounts:
        get_account(account)
    print("================================================\n\n")

    print("=================PAID ACCOUNTS==================")
    print("| Account | Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |")
    print("| ------- | ------- | --------- | ----------- | ----------- |")
    for account in paid_accounts:
        get_account(account, False)
    print("================================================")

    for r in refundings:
        print(r)
