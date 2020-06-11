import requests
from envelope import Envelope
import pickle
import time
from transaction import Transaction
from block import Block
import block


ADDRESS = "http://127.0.0.1:5000/"


#returns the entire blockchain
def get_blockchain():
    r = requests.get(ADDRESS + "blockchain")
    bc = pickle.loads(r.content)
    return bc

#returns list of transactions
def get_transactions():
    r = requests.get(ADDRESS + "transactions")
    trans_list = pickle.loads(r.content)
    return trans_list

# length of blockchain and number of transactions is returned in an envelope class
def check_in():
    r = requests.get(ADDRESS + "check_in")
    envelope = pickle.loads(r.content)
    return envelope

#add a block to the blockchain if failed it means that your blockchain wrong and need to update the the correct one
#not sure if we are passing the whole block chain or just the block
def add_block(blockchain):
    pickled_blockchain = pickle.dumps(blockchain)
    r = requests.post(ADDRESS + "blockchain", data = pickled_blockchain)
    response = r.content.decode()
    if(response == "success"):
        return response
    else:
        #failed to write to blockchain need to get new block and current transactions
        return response

def add_transaction(transaction):
    pickled_transaction = pickle.dumps(transaction)
    r = requests.post(ADDRESS + "transactions", data = pickled_transaction)
    response = r.content.decode()

    if(response == "success"):
        return "success"
    else:
        return "failed"

##################################################################################################################
def main():
     #on start up pull transaction and block
     client_running = True
     do = 1
     max_loop = 200000
     sleep_time = 5
     print("Hello this is the miner starting up")
     print("i am going to get the blockchain and transactions from the server")

     chain = get_blockchain()
     transactions = get_transactions()

     while client_running:

        while len(transactions) < block.MIN_TRANSACTIONS_PER_BLOCK:
            print(f"Waiting for more Xacts. Sleeping for {sleep_time} sec.")
            time.sleep(sleep_time)
            transactions = get_transactions()

        proof_of_work_trans = transactions[:block.MAX_TRANSACTIONS_PER_BLOCK]

        #python does not have a do while loop I made my own
        #this is the mining process
        while True:
            prev_proof = chain.data[-1].proof
            results = chain.data[-1].proof_of_work(prev_proof, proof_of_work_trans)

            if(results != None):
                #generate block
                new_block = Block() #figure out how to call client
                new_block.timestamp = proof_of_work_trans[-1].timestamped_msg.timestamp
                new_block.transactions = proof_of_work_trans

                new_block.proof = results
                #hash
                chain.add(new_block)
                results = add_block(chain)
                if(results == "success"):
                    print(f"I just successfully mined a block with id {str(new_block.hash())[-10:]}")
                    do = max_loop + 1 #break do while loop
                    print(f"now the number of my transactions is going to go down from {len(transactions)} to {check_in().transactions_count}")
                    transactions = get_transactions()
                else:
                    print("I generated a block but failled to push it to the server")
                    print("i must now update my current blockchain and transactions")
                    do = max_loop + 1 #break do while loop and will go to check_in section to get the new chain and transactions

                #check to see if push was succesful
                break

            do = do + 1
            if (do > max_loop):
                break

        #do a check in
        print("doing a check in!")
        check = check_in()
        if check.blockchain_len > chain.length:
            print(f"I am getting a new blockchain. Mine was length {chain.length}.")
            chain = get_blockchain()
            print(f"My new blockchain is of length {chain.length}")
            transactions = get_transactions()
        #transactions just got updated
        elif len(transactions) != check.transactions_count:
            print(f"There are {check.transactions_count} transactions, and I only have {len(transactions)} of them. I am getting the rest.")
            transactions = get_transactions()

        do = 1 #reset the do while loop

###################################################################################################

main()
