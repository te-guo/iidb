In $\texttt{table.h}$​​, new index-related APIs are provided (indexes do not support duplicate keys):

- $\texttt{has\_index(idx)}$ is used to check whether a table has an index built on its $\texttt{idx}$-th field.
- $\texttt{build\_index(idx)}$ is used to build an index on the table's $\texttt{idx}$-th field.
- $\texttt{delete\_index(idx)}$ is used to delete the index on the table's $\texttt{idx}$-th field.

(The following functions assume that there is an index already built on the table's $\texttt{idx}$​-th field.)

- $\texttt{key\_has(idx, key)}$ is used to check whether there is a record whose $\texttt{idx}$-th field is $\texttt{key}$.
- $\texttt{key\_at(idx, key)}$ is used to find the entry whose record's $\texttt{idx}$-th field is $\texttt{key}$.
- $\texttt{key\_range(idx, lower, upper)}$ is used to find all the entries whose record's $\texttt{idx}$-th field is $\ge\texttt{lower}$ and $<\texttt{upper}$.
- $\texttt{insert\_with\_index(idx, record)}$ is used to insert $\texttt{record}$ into the table.
- $\texttt{update\_with\_index(idx, key, record)}$ is used to update the record whose $\texttt{idx}$-th field is $\texttt{key}$ with $\texttt{record}$.
- $\texttt{remove\_with\_index(idx, key)}$ is used to remove the record whose $\texttt{idx}$-th field is $\texttt{key}$ from the table.

