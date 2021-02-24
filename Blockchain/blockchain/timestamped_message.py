class TimestampedMessage:
    def __init__(self, message: bytes, timestamp: int):
        self.message = message
        self.timestamp: int = timestamp

    def __gt__(self, other):
        if isinstance(other, TimestampedMessage):
            if self.timestamp == other.timestamp:
                return self.message > other.message
            return self.timestamp > other.timestamp
        return False
    
    def __lt__(self, right_hand_side):
        if not isinstance(right_hand_side, TimestampedMessage):
            return False
        if self.timestamp != right_hand_side.timestamp:
            return self.timestamp < right_hand_side.timestamp
        return self.message < right_hand_side.message

    def __eq__(self, other):
        if isinstance(other, TimestampedMessage):
            return self.timestamp == other.timestamp
        return False

    def to_bytes(self):
        result = bytearray(self.message)
        result.extend(str(self.timestamp).encode())
        return bytes(result)

    def timestamp_in_left_open_interval(self, lower_bound, upper_bound):
        gt_lower_bound = self.timestamp > lower_bound
        lt_eq_upper_bound = self.timestamp <= upper_bound
        
        if gt_lower_bound and lt_eq_upper_bound:
            return True
        print(f"Timestamp outside of specified interval. \
                lower_bound: {lower_bound}, \
                timestamp: {self.timestamp}, \
                upper_bound: {upper_bound}")
        return False
