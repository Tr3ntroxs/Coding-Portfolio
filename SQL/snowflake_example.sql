use role accountadmin;

-- DDL
create or replace database trent_database;

create or replace schema sqltutorial;

-- Create table example with all data types
create or replace table employees (

    employee_id INTEGER AUTOINCREMENT,
    FIRST_name STRING,
    last_name STRING,
    department_id INTEGER,
    salary DECIMAL(10,2),
    hire_date DATE,
    PRIMARY KEY(employee_id)
);

use database SNOWFLAKE_SAMPLE_DATA;

use schema TPCDS_SF100TCL;

select * from CALL_CENTER;

use role accountadmin;

use database trent_database;

use schema sqltutorial;

-- DQL

select * from employees;

insert into employees (first_name, last_name, department_id, salary, hire_date)
values ('Raj', 'Kanth', 1, 10000, '2024-07-19'),
('Raj', 'Kanth', 1, 10000, '2024-07-15');

select * from employees;
