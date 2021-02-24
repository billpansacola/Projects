import rsa
import hashlib
import time
from timestamped_message import TimestampedMessage

class Transaction:
    """
    # :param author: an RSA public key
    # :param timestamped_msg: the unencrypted message to post
    # :param signature: the result of signing timestamped_msg.to_bytes() with the author's RSA private key
    """
    HASH_METHOD = 'SHA-256'

    def __init__(
        self,
        author: rsa.PublicKey,
        timestamped_msg: TimestampedMessage,
        signature: bytes
    ):
        self.author = author
        self.timestamped_msg = timestamped_msg
        self.signature = signature

    @staticmethod
    def create_with_keys(pub: rsa.PublicKey, priv: rsa.PrivateKey, body: bytes,
        timestamp: int
    ):
        author = pub
        timestamped_msg = TimestampedMessage(body, timestamp)
        signature = \
            rsa.sign(timestamped_msg.to_bytes(), priv, Transaction.HASH_METHOD)
        return Transaction(author, timestamped_msg, signature)

    def verify(self, timestamp_of_latest_block: int) -> bool:
        try:
            rsa.verify(
                self.timestamped_msg.to_bytes(), self.signature, self.author
            )
        except rsa.VerificationError:
            print("Signature invalid!")
            return False
        now = time.time_ns()
        timestamp_is_ok = self.timestamped_msg.timestamp_in_left_open_interval(
            timestamp_of_latest_block,
            now
        )
        if not timestamp_is_ok:
            print("Timestamp is bad!")
        return timestamp_is_ok

    def hash(self):
        sha_summer = hashlib.sha256()
        sha_summer.update(str(self.author.n).encode("utf-8"))
        sha_summer.update(str(self.author.e).encode("utf-8"))
        sha_summer.update(self.timestamped_msg.to_bytes())
        return sha_summer.digest()

    def __gt__(self, right_hand_side):
        if isinstance(right_hand_side, Transaction):
            if self.timestamped_msg == right_hand_side.timestamped_msg:
                return self.author > right_hand_side.author
            return self.timestamped_msg > right_hand_side.timestamped_msg
        return False

    def __lt__(self, right_hand_side):
        if isinstance(right_hand_side, Transaction):
            if self.timestamped_msg == right_hand_side.timestamped_msg:
                return self.author < right_hand_side.author
            return self.timestamped_msg < right_hand_side.timestamped_msg
        return False

    def __eq__(self, right_hand_side):
        if isinstance(right_hand_side, Transaction):
            return self.signature == right_hand_side.signature
        return False
