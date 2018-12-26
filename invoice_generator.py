import datetime
import random
from faker import Faker
import sqlite3

Price = {
   1     :      300.00  ,
   2     :      150.00  ,
   3     :      300.00  ,
   4     :      45.00  ,
   5     :      200.00  ,
   6     :      3000.00 ,
   7     :      800.00 ,
   8     :      600.00  ,
   9     :      400.00  ,
   10    :      5000.00 ,
   11    :      5100.00 ,
   12    :      4000.00 ,
   13    :      15.00  ,
   14    :      350.00 ,
   15    :      10.0
}


fake = Faker()
def func():
   ClientName = fake.name()
   ClientAddress = fake.address()
   BillingAmount = 0.0
   GstAmount = round(random.randint(1, 10) * 100, 2)
   ShipAmount = round(random.randint(1, 10) * 100, 2)

   ProductID = []
   Qty = []

   for i in range(5):
      ProductID.append(random.randint(1, 15))
      Qty.append(random.randint(1, 10))
      BillingAmount += Price[ProductID[i]] * Qty[i]

   BillingAmount = round(BillingAmount, 2)
   ProductList = '{'
   for i in range(0, len(ProductID)):
      if (i != len(ProductID) - 1):
         ProductList += '"' + str(ProductID[i]) + '":' + str(Qty[i]) + ','
      else:
         ProductList += '"' + str(ProductID[i]) + '":' + str(Qty[i])
   ProductList += '}'

   day_offset = random.randint(1, 30)
   Date = (datetime.date.today() + datetime.timedelta(days = day_offset))
   IssueDate = Date.strftime('%a %b %d %Y')
   DueDate = (Date + datetime.timedelta(days = day_offset + random.randint(1, 7))).strftime('%a %b %d %Y')

   return (ClientName, ClientAddress, ProductList, IssueDate, DueDate, BillingAmount, GstAmount, ShipAmount,
      (BillingAmount))
   

conn = sqlite3.connect('smartbill.db')
cursor = conn.cursor()


for i in range(49):
   result = func()
   print(result)
   cursor.execute('''insert into InvoiceInfo (ClientName, ClientAddress, ProductList, IssueDate, DueDate,
      BillingAmount, GstAmount, ShipAmount, PaidAmount) values (?, ?, ?, ?, ?, ?, ?, ?, ?)''', result)

conn.commit()