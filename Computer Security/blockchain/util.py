import datetime

def dictToObj(dict): # converts dict to object for use in json.load()
    moduleName = dict.pop("module") # get module name from dict and pop it
    className = dict.pop("class") # get class name from dict and pop it
    type = getattr(__import__(moduleName), className) # load class from module

    return type(**dict) # return object of class by unpacking dictionary

def objToDict(object): # convert object to dictionary for use in json.dump()
    dict = {} # create dictionary to fill with object information

    if isinstance(object, datetime.datetime): # if object is a datetime.date
        dict = {"year": object.year, "month": object.month, "day": object.day, "hour": object.hour, "minute": object.minute, "second": object.second, "microsecond": object.microsecond}
        dict.update({"module": "datetime"}) # add module for recovery
        dict.update({"class": "datetime"}) # and class name
    else: # otherwise extract information to dict for the general case
        dict = object.__dict__ # convert object to dictionary
        dict.update({"module": object.__module__}) # add module for recovery
        dict.update({"class": object.__class__.__name__}) # and class name

    return dict # returns a dictionary version of object
