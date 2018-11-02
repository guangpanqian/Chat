use mysql;
create table Employee(
	id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name  varchar(6),
    password varchar(9)
);

create table employee_offline_message(
	id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    idfrom INT,
    idto INT,
    sentence varchar(100)
)


