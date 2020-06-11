import time
import hashlib                 # for SHA256 cryptographic hashing
from transaction import Transaction
import random

MAX_TRANSACTIONS_PER_BLOCK=3
MIN_TRANSACTIONS_PER_BLOCK=3
ENOUGH_ZEROS_FOR_A_PROOF_OF_WORK = "00000"

class Block:
    def __init__(self, timestamp=time.time_ns(), blockHash=None, index=1, previousBlockHash=None, proof=None, transactions=[]):

        # initially when block is created, updates to latest transaction time
        self.timestamp = timestamp

        self.transactions = transactions # the list of the block's transactions
        self.previousBlockHash = previousBlockHash # previous block's hash
        self.index = index # index of the block in the chain
        self.proof = proof # proof of block (initially 100 for Genesis Block)

        if blockHash == None: # if no hash is provided, it is auto-generated
            self.blockHash = self.hash()
        else:
            self.blockHash = blockHash

    def hash(self): # returns the SHA256 hash of the block
        hasher = hashlib.sha256()

        # add each component of the block to the hasher
        hasher.update(str(self.timestamp).encode('utf-8'))
        hasher.update(str(self.index).encode('utf-8'))
        hasher.update(str(self.previousBlockHash).encode('utf-8'))

        # include each transaction hash in block hash
        for transaction in self.transactions:
            hasher.update(transaction.hash())

        return hasher.hexdigest() # return the hexidecimal representing the hash

    def addTransaction(self, transactionToAdd): # adds transaction to the block
        self.transactions.append(transactionToAdd) # append new transaction
        self.transactions.sort()

        # Update timestamp to latest transaction in the list
        self.timestamp = self.transactions[-1].timestamped_msg.timestamp
        self.blockHash = self.hash() # update hash of block

    def verify(self, previousTimestamp, previousProof):
        # start data for use in proof of work verification using previousProof
        data = bytearray(str(previousProof).encode())
        print("Verifying block ", self.index)

        for i in range(0, len(self.transactions)): # loop through transactions
            # verify each transaction in the block
            if self.transactions[i].verify(previousTimestamp) == False:
                print("Verification failed: unverified transaction in block")
                return False
            # ensure each transaction != to any other transaction in the block
            if i + 1 == len(self.transactions) - 1:
                if self.transactions[i] == self.transactions[i+1]:
                    print("Verification failed: identical transactions in block")
                    return False
            elif i != len(self.transactions) - 1:
                for j in (i+1, len(self.transactions) - 1):
                    if self.transactions[i] == self.transactions[j]:
                        print("Verification failed: identical transactions in block")
                        return False
            # add each transaction to data for use in proof of work verification
            data.extend(self.transactions[i].hash())

        if not self.hash_proof(self.proof, data): # verify proof of work
            print("Verification failed: PoW hash didn't produce leading 0s")
            return False

        # verify number of transactions in block is satisfactory
        if len(self.transactions) >  MAX_TRANSACTIONS_PER_BLOCK:
            print("Verification failed: too many transactions in block")
            return False
        if len(self.transactions) < MIN_TRANSACTIONS_PER_BLOCK:
            print("Verification failed: too few transactions in block")
            return False

        return True

    def print(self): # prints the parameters of the block line by line
        print("Index: ", self.index)
        print("Timestamp: ", self.timestamp)
        print("Hash: ", self.blockHash)
        print("Previous Blocks Hash: ", self.previousBlockHash)
        print("Proof: ", self.proof)

        for transaction in self.transactions:
            print(transaction.author)
            print(transaction.timestamped_msg)
            print(transaction.signature)

    def proof_of_work(self, prev_proof, transactions_to_be_mined): # generates a proof for a block
        data_in_hash = bytearray(str(prev_proof).encode())
        for transaction in transactions_to_be_mined:
            data_in_hash.extend(transaction.hash())

        guess_at_this_blocks_proof_number = random.random()
        if self.hash_proof(guess_at_this_blocks_proof_number, data_in_hash):
            return guess_at_this_blocks_proof_number
        return None

    def hash_proof(self, guess, data: bytearray):
        f = bytearray(data)
        f.extend(str(guess).encode())
        data_with_guess = f
        hash = hashlib.sha256(str(data_with_guess).encode()).hexdigest()
        return hash.startswith(ENOUGH_ZEROS_FOR_A_PROOF_OF_WORK)
