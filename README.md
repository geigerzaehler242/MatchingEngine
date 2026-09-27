An Order book Matching engine for trading instruments

accepts orders and cancels from a udp port, 
maintains multiple price-time order books one per symbol, 
publishes acknowledgements, 
trades and top of book changes.


supporting three types of transactions: new order ’N', cancel order ‘C’ and flush ‘F’ or-derbook.

Order Book Processing

Order book is price-time:
•	market orders take the opposite side immediately, unmatched portion assume can-celled (fill and kill)
•	limit orders (if not matched immediately) join the book in time priority
•	partial quantity matches are possible

Publish on separate thread:
•	to console/stdout
•	publish order or cancel acknowledgement format:
	A, userId, userOrderId
•	publish trades (matched orders) format:
	T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
•	publish changes in Top Of Book per side using format, use ‘-‘ for side elimination:
	B, side (B or S), price, totalQuantity
