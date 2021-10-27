Database: MySQL

#### Dataset

```sql
create database lab1;
use lab1;
source schema.sql;
source data.sql;
```

#### Single-table queries

1. 

```sql
select O_CUSTKEY, sum(O_TOTALPRICE)
from ORDERS
group by O_CUSTKEY
having count(*) > 20;
```

2. 

```sql
update LINEITEM
set L_TAX = L_TAX * 1.1
where L_DISCOUNT > 0.02;
```

3. 

```sql
select L_ORDERKEY, avg(L_DISCOUNT) as avg_discount
from LINEITEM
where L_TAX < 0.05
group by L_ORDERKEY
order by avg_discount desc
limit 10;
```

4. 

```sql
select L1.L_ORDERKEY as L_ORDERKEY, L1.L_LINENUMBER as L_LINENUMBER
from LINEITEM as L1
where L1.L_DISCOUNT >= all(select L2.L_DISCOUNT from LINEITEM as L2);
```

5. 

```sql
select PS_PARTKEY, sum(PS_AVAILQTY)
from PARTSUPP
group by PS_PARTKEY;
```

#### Multi-table queries

1. 

   For each order whose customer's nation is China, print the name of its customer, its status, and the key of its customer's nation.

2. 

```sql
select *
from CUSTOMER
where CUSTOMER.C_CUSTKEY in (select ORDERS.O_CUSTKEY from ORDERS group by ORDERS.O_CUSTKEY having sum(ORDERS.O_TOTALPRICE) < 10000);
```

3. 

```sql
select L_ORDERKEY, L_LINENUMBER, C_NAME, O_ORDERSTATUS, N_NATIONKEY
from LINEITEM, CUSTOMER, ORDERS, NATION
where L_ORDERKEY = O_ORDERKEY and C_CUSTKEY = O_CUSTKEY and C_NATIONKEY = N_NATIONKEY and N_NAME = 'CHINA';
```

- Select the records where N_NAME = 'CHINA' from table NATION
- Join the result table with table CUSTOMER on C_NATIONKEY = N_NATIONKEY
- Join the result table with table ORDERS on C_CUSTKEY = O_CUSTKEY
- Join the result table with table LINEITEM on L_ORDERKEY = O_ORDERKEY
- Project the result table on the expected attributes
