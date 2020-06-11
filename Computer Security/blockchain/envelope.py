class Envelope:
    def __init__(self, blockchain_len, transactions_count):
        self.blockchain_len = blockchain_len
        self.transactions_count = transactions_count
    
    def e_print(self):
        print(self.blockchain_len)
        print(self.transactions_count)
