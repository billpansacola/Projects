import block
from transaction import Transaction
import util
import hashlib
import random

import json # used for loading and saving json data

TIMESTAMP_FOR_GENESIS_BLOCK: int = 0

class Chain:
    def __init__(self, length=0, data=[]):
        self.length = length # keeps track of the length of the chain
        self.data = data # the actual list of blocks in the chain

        if length == 0: # if length==0, the chain is new, so add a genesis block
            genesisBlock = self.generateGenesisBlock()
            self.data = [genesisBlock]

    def generateGenesisBlock(self): # creates and returns a genesis block
        self.length += 1
        return block.Block(TIMESTAMP_FOR_GENESIS_BLOCK)

    def add(self, blockToAdd: block.Block): # adds a new block to the blockchain
        self.length += 1 # increment blockchain length variable

        # calculate the hash of last block in the chain
        blockToAdd.previousBlockHash = self.data[-1].hash()

        blockToAdd.index = self.length # the index will be the new length
        blockToAdd.blockHash = blockToAdd.hash() # update hash of new block
        self.data.append(blockToAdd) # append the block to the blockchain

    def print(self): # prints out all of the blocks in the blockchain
        for block in self.data:
            block.print()
            print()

    def save(self): # saves the entire blockchain to file as JSON
        filePath = "json/blockchain.json" # construct path to output
        jsonFile = open(filePath, 'w') # open JSON output file and dump data
        json.dump(self, jsonFile, default = util.objToDict)
        jsonFile.close() # close the JSON file after dump

    def load(self): # loads the blockchain in the file from JSON
        filePath = "json/blockchain.json" # construct path to JSON file
        jsonFile = open(filePath, 'r') # read file and add to data list
        blockchain = json.load(jsonFile, object_hook = util.dictToObj)
        return blockchain

    def verify(self):
        print("Verifying the chain.")
        for i in range(0, len(self.data) - 1):
            if self.data[i + 1].verify(self.data[i].timestamp, self.data[i].proof) == False:
                return False
        print("All blocks verified.")

        if len(self.data) != self.length:
            print("Verification failed: mismatched lengths in chain")
            return False
        print("Length verified.")

        if len(self.data[0].transactions) > 0:
            print("Verification failed: transactions in genesis block")
            return False
        print("Genesis block (block 1) verified.")

        return True
