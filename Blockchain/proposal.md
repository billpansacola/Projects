# **Project Proposal**
### *written by Bill Pansacola, Ben Rausch, Connor Bailey, and Dane Archer*
----

## **What problem are we solving?**
Human knowledge in the form of research papers is not organized well. This is because research is published across many venues, and the relationships between papers is hard to infer without manual review. As a consequence, when research is overturned by new research or failure to verify findings, the fallout is unclear. News outlets may report outdated research for years to come, and how will researchers learn when a paper they have cited is overturned? There is no clear path to convergence upon new knowledge.

## **Why is this problem important?**
New information is being published almost every day that affects our lives -- nutrition, the security of services we use daily, et cetera. Not to mention the multibillion-dollar industry of forming new knowledge, which relies upon having up-to-date information to build upon. If we are relying upon outdated information, someone could lose their health (relying upon outdated nutrition or medical research) or their life (relying upon outdated medical or security research).

## **What do we plan to build**
A blockchain capable of addressing this problem.
There is no indisputable, public ledger of research papers, findings, overturning of conclusions, retractions, challenges, peer reviews, and citations. If there was, it would promote a more accurate network of information by keeping all information “in the light” and allowing computation on the data. Once a paper is challenged, we could automatically notify anyone who has cited it before. We would also be able to preserve a record of debate over research papers for future generations to benefit from.


## **Our expected results**
A C program that acts as a node for our blockchain. In order to implement our blockchain, we must also implement a P2P network to store/retrieve list of peers and blockchains, sign and publish transactions, verification of transactions, and mining blocks. Transactions will be arbitrary strings which we would impose semantic meaning upon to implement our ledger. We expect to be able to tell which of two papers was published first, to a margin of 30 minutes.