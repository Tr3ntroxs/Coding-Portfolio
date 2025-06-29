-- CREATE DATABASE test;
-- USE test;


-- Test run

CREATE TABLE test_table (
    test_column INT
);

ALTER TABLE test_table
ADD col VARCHAR(25);

DROP TABLE test_table;

-- Record example

-- Create tables
CREATE TABLE bands (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(255) NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE albums (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(255) NOT NULL,
    release_year INT, 
    band_id INT NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (band_id) REFERENCES bands(id)
);

-- Insert one entry
INSERT INTO bands (name)
VALUES ('Iron Maiden');

-- Insert multiple entries
INSERT INTO bands (name)
VALUES ('Deuce'), ('Avenged Sevenfold'), ('Ankor');

-- Select ALL columns
SELECT * FROM bands;

-- Select columns up to 2 rows
SELECT * FROM bands LIMIT 2;

-- Select row
SELECT name FROM bands;

-- Change column names
SELECT id AS 'ID', name AS 'Band Name' FROM bands;

-- Order by normal (ASC by default), reverse alphabetical order
SELECT * FROM bands ORDER BY name ASC;
SELECT * FROM bands ORDER BY name DESC;
SELECT * FROM bands ORDER BY name;

INSERT INTO albums (name, release_year, band_id)
VALUES ('The Number of the Beasts', 1985, 1),
        ('Power Slave', 1984, 1),
        ('Nightmare', 2018, 2),
        ('Nightmare', 2010, 3),
        ('Test Album', NULL, 3);

SELECT * FROM albums;

-- Unique entries
SELECT DISTINCT name FROM albums;

-- Update entry
UPDATE albums
SET release_year = 1982
WHERE id = 1;

-- Filter
SELECT * FROM albums
WHERE release_year < 2000;

-- Filter for substring 'er' or specific values
SELECT * FROM albums
WHERE name LIKE '%er%' OR band_id = 2;

SELECT * FROM albums
WHERE release_year = 1984 AND  band_id = 1;

SELECT * FROM albums
WHERE release_year BETWEEN 2000 AND 2018;

SELECT * FROM albums
WHERE release_year IS NULL;

DELETE FROM albums WHERE id = 5;

SELECT * FROM albums;

-- Joins entries of similar id into one table
SELECT * FROM bands
INNER JOIN albums ON bands.id = albums.band_id;

-- Joins all entries of either table (left or right)
SELECT * FROM bands
LEFT JOIN albums ON bands.id = albums.band_id;

SELECT * FROM albums
RIGHT JOIN albums ON bands.id = albums.band_id;

-- Aggregate
SELECT AVG(release_year) FROM albums;
SELECT SUM(release_year) FROM albums;

-- Use of count in selecting
SELECT band_id, COUNT(band_id) FROM albums
GROUP BY band_id;

-- Alias
SELECT b.name AS band_name, COUNT(a.id) AS num_albums
FROM bands AS b
LEFT JOIN albums AS a ON b.id = a.band_id
WHERE b.name = 'Deuce' -- Cannot be used with count which occurs after grouping
GROUP by b.id
HAVING num_albums = 1; -- That's why we use having instead