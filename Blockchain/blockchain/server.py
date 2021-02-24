from flask import Flask
from flask import request
from envelope import Envelope
import pickle
from transaction import Transaction
from chain import Chain
from block import Block
import bisect


blockchain = Chain() ## on start up create origin block with block static methond With @staticmethod of Block class
transactions =[] #sorted oldest to youngest... oldest has a smaller time stamp

total_time_spent_mining = 0
blocks_mined = 0

app = Flask(__name__)

@app.route("/")
def home():
    return "Hello! this is the main page"

@app.route("/blockchain", methods=["GET","POST"])
def g_p_blockchain():
    global blockchain
    global transactions
    if request.method == 'GET':
        return pickle.dumps(blockchain)

    if request.method == 'POST':
        new_bc = request.data
        new_bc = pickle.loads(new_bc)

        if (new_bc.length > blockchain.length and new_bc.verify()):
            blockchain = new_bc
            #Now removing transactions that have a timestamp older than the youngest block in the chain

            timestamp_of_last_block = blockchain.data[-1].timestamp
            trans_temp = []
            for val in transactions:
                if val.timestamped_msg.timestamp > timestamp_of_last_block:
                    trans_temp.append(val)
            transactions = trans_temp
            if (new_bc.length > 2): # we have already mined 1 block, so clients are up and running
                global blocks_mined
                global total_time_spent_mining
                blocks_mined += 1
                time_spent_mining_this_block = new_bc.data[-1].timestamp - new_bc.data[-2].timestamp
                total_time_spent_mining += time_spent_mining_this_block
                print(f"{blocks_mined}th block after the warm-up block was mined in {time_spent_mining_this_block/1e9} s. AVG={total_time_spent_mining / (blocks_mined*1e9)} s.")


            return "success"

        else:
            return "failed"

@app.route("/transactions", methods=["GET", "POST"])
def g_p_transactions():
    global blockchain
    global transactions
    if request.method == 'GET':

        return pickle.dumps(transactions)

    if request.method == 'POST':
        new_transaction = request.data
        new_transaction = pickle.loads(new_transaction)

        time_stamp = blockchain.data[-1].timestamp
        transaction_is_valid = new_transaction.verify(time_stamp) #true or false

        if transaction_is_valid:
            #insert new_transaction into a sorted list
            #https://stackoverflow.com/questions/26840413/insert-a-custom-object-in-a-sorted-list
            bisect.insort_right(transactions, new_transaction)
            #print for check
            for i in transactions:
                print(str(i.author.n)[-10:] + " " + str(i.timestamped_msg.timestamp))

            return "success"
        else:
            return "failed"

@app.route("/check_in", methods=["GET"])
def check_in(): #gives length of current block chain and how many current transactions
    envelope = Envelope(blockchain.length, len(transactions))
    return pickle.dumps(envelope)

if __name__ == "__main__":
    app.run(debug=True)
