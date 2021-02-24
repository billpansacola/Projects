import block
import chain

def generateNewBlockchain(): # to be used when the user wants to generate a new blockchain
    blockchain = chain.Chain()
    return blockchain

def loadBlockchain(): # to be used when a user wants to load a blockchain
    blockchain = chain.Chain()
    return blockchain.load()

def test(): # tester function to test/demo basic blockchain functionality
    blockchain = generateNewBlockchain() # generate new blockchain

    # add two blocks to the newblockchain
    paper1 = "There has been a recent resurgence in the area of explainable artificial intelligence as researchers and practitioners seek to provide more transparency to their algorithms. Much of this research is focused on explicitly explaining decisions or actions to a human observer, and it should not be controversial to say that looking at how humans explain to each other can serve as a useful starting point for explanation in artificial intelligence. However, it is fair to say that most work in explainable artificial intelligence uses only the researchers’ intuition of what constitutes a ‘good’ explanation. There exist vast and valuable bodies of research in philosophy, psychology, and cognitive science of how people define, generate, select, evaluate, and present explanations, which argues that people employ certain cognitive biases and social expectations to the explanation process. This paper argues that the field of explainable artificial intelligence can build on this existing research, and reviews relevant papers from philosophy, cognitive psychology/science, and social psychology, which study these topics. It draws out some important findings, and discusses ways that these can be infused with work on explainable artificial intelligence."
    block1 = block.Block(paper1)
    blockchain.add(block1)
    paper2 = "Recently, the notion of explainable artificial intelligence has seen a resurgence, after having slowed since the burst of work on explanation in expert systems over three decades ago; for example, see Chandrasekaran et al. [23], [168], and Buchanan and Shortliffe [14]. Sometimes abbreviated XAI (eXplainable artificial intelligence), the idea can be found in grant solicitations [32] and in the popular press [136]. This resurgence is driven by evidence that many AI applications have limited take up, or are not appropriated at all, due to ethical concerns [2] and a lack of trust on behalf of their users [166,101]. The running hypothesis is that by building more transparent, interpretable, or explainable systems, users will be better equipped to understand and therefore trust the intelligent agents [129,25,65]. While there are many ways to increase trust and transparency of intelligent agents, two complementary approaches will form part of many trusted autonomous systems: (1) generating decisions1 in which one of the criteria taken into account during the computation is how well a human could understand the decisions in the given context, which is often called interpretability or explainability; and (2) explicitly explaining decisions to people, which we will call explanation. Applications of explanation are considered in many sub-fields of artificial intelligence, such as justifying autonomous agent behaviour [129,65], debugging of machine learning models [89], explaining medical decision-making [45], and explaining predictions of classifiers [157]."
    block2 = block.Block(paper2)
    blockchain.add(block2)

    blockchain.print() # print the blockchain
    blockchain.save()  # save it to local file
    bc = loadBlockchain() # load the blockchain
    bc.print() # print the loaded blockchain

test()
