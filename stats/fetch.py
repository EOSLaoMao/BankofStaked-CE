import pprint
import time
from eosapi import Client

c = Client(nodes=['https://geo.eosasia.one'])

def check_order(lower_bound=1):
    expired_orders = []
    now = time.time()
    new_lower_bound = lower_bound
    r = c.get_table_rows(**{"code": "bankofstaked", "scope": "921459758687", "table": "order", "json": True, "limit": 100, "upper_bound": None, "lower_bound": lower_bound, "table_key": "id"})
    more = r["more"]
    total_count = 0
    count = 0
    free_count = 0
    paid = []
    for line in r["rows"]:
        total_count += 1
        if line["expire_at"] < now:
            expired_orders.append(line)
            count+=1
            #print(line)
            if line["is_free"]:
                #print("free id:", line["id"])
                free_count += 1
            else:
                paid.append(line["id"])
        if line["id"] > lower_bound:
            new_lower_bound = line["id"]
    print("total orders: %d" % total_count)
    print("expired orders: %d" % count)
    print("expired free orders: %d" % free_count)
    return more, new_lower_bound, expired_orders



def fetch_creditors():
    paid_accounts = []
    free_accounts = []
    r = c.get_table_rows(**{"code": "bankofstaked", "scope": "921459758687", "table": "creditor", "json": True, "limit": 1000, "upper_bound": None, "lower_bound": None, "table_key": "account_name"})

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
    row = " | ".join([r["account_name"], str("%.4f EOS" % liquid_balance), str("%.4f EOS" % balance), str("%.2f" % (ram_quota/1024.)), str("%.2f" % ram_required), "✅" if (ram_quota/1024. > ram_required) else "❌"])
    row = "| %s |" % row
    print(row)
    return balance, liquid_balance


if __name__ == "__main__":
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
    paid_ids = set()
    ids = set()
    for e in expired:
        bps.add(e["creditor"])

        if e["is_free"]:
            ids.add(" ".join([e["creditor"], str(e["id"])]))
        else:
            paid_ids.add(" ".join([e["creditor"], str(e["id"])]))
    ids = list(ids)
    paid_ids = list(paid_ids)
    ids.sort()
    paid_ids.sort()
    print("total expired ids:", len(ids), ids)
    print("paid expired ids:", len(paid_ids), paid_ids)
    f1 = open("expired_order_ids.txt", "w")
    f1.write("\n".join([str(i) for i in ids]))
    f2 = open("expired_paid_order_ids.txt", "w")
    f2.write("\n".join([str(i) for i in paid_ids]))
    refundings = []
    free_accounts, paid_accounts = fetch_creditors()
    print("=================FREE ACCOUNTS==================")
    print("| Account | Liquid Balance | Total Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |")
    print("| ------- | ------- | --------- | --------- | ----------- | ----------- |")
    for account in free_accounts:
        get_account(account)
    print("================================================\n\n")

    print("=================PAID ACCOUNTS==================")
    print("| Account | Liquid Balance | Total Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |")
    print("| ------- | ------- | --------- | --------- | ----------- | ----------- |")
    total_balance = 0
    total_liquid_balance = 0
    for account in paid_accounts:
        balance, liquid_balance = get_account(account, False)
        total_balance += balance
        total_liquid_balance += liquid_balance
    print("total balance:", total_balance)
    print("total liquid balance:", total_liquid_balance)
    print("================================================")

    for r in refundings:
        print(r)
