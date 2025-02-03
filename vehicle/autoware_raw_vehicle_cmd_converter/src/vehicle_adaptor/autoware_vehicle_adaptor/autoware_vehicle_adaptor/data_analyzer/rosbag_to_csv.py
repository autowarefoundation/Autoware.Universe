import argparse
import array
import csv
import glob
import os
import re
import sqlite3

import numpy as np
import pandas as pd
from rclpy.serialization import deserialize_message
from rosidl_runtime_py.utilities import get_message


def connect(sqlite_file):
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    return conn, c


def close(conn):
    conn.close()


def countRows(cursor, table_name, print_out=False):
    """Returns the total number of rows in the database."""
    cursor.execute("SELECT COUNT(*) FROM {}".format(table_name))
    count = cursor.fetchall()
    if print_out:
        print("\nTotal rows: {}".format(count[0][0]))
    return count[0][0]


def getHeaders(cursor, table_name, print_out=False):
    """Returns a list of tuples with column informations:
    (id, name, type, notnull, default_value, primary_key)
    """
    # Get headers from table "table_name"
    cursor.execute("PRAGMA TABLE_INFO({})".format(table_name))
    info = cursor.fetchall()
    if print_out:
        print("\nColumn Info:\nID, Name, Type, NotNull, DefaultVal, PrimaryKey")
        for col in info:
            print(col)
    return info


def getAllElements(cursor, table_name, print_out=False):
    """Returns a dictionary with all elements of the table database."""
    # Get elements from table "table_name"
    cursor.execute("SELECT * from({})".format(table_name))
    records = cursor.fetchall()
    if print_out:
        print("\nAll elements:")
        for row in records:
            print(row)
    return records


def isTopic(cursor, topic_name, print_out=False):
    """Returns topic_name header if it exists. If it doesn't, returns empty.
    It returns the last topic found with this name.
    """
    boolIsTopic = False
    topicFound = []

    # Get all records for 'topics'
    records = getAllElements(cursor, "topics", print_out=False)

    # Look for specific 'topic_name' in 'records'
    for row in records:
        if row[1] == topic_name:  # 1 is 'name' TODO
            boolIsTopic = True
            topicFound = row
    if print_out:
        if boolIsTopic:
            # 1 is 'name', 0 is 'id' TODO
            print("\nTopic named", topicFound[1], " exists at id ", topicFound[0], "\n")
        else:
            print("\nTopic", topic_name, "could not be found. \n")

    return topicFound


def getAllMessagesInTopic(cursor, topic_name, print_out=False):
    """Returns all timestamps and messages at that topic.
    There is no deserialization for the BLOB data.
    """
    count = 0
    timestamps = []
    messages = []

    # Find if topic exists and its id
    topicFound = isTopic(cursor, topic_name, print_out=False)

    # If not find return empty
    if not topicFound:
        print("Topic", topic_name, "could not be found. \n")
    else:
        records = getAllElements(cursor, "messages", print_out=False)

        # Look for message with the same id from the topic
        for row in records:
            if row[1] == topicFound[0]:  # 1 and 0 is 'topic_id' TODO
                count = count + 1  # count messages for this topic
                timestamps.append(row[2])  # 2 is for timestamp TODO
                messages.append(row[3])  # 3 is for all messages

        # Print
        if print_out:
            print("\nThere are ", count, "messages in ", topicFound[1])

    return timestamps, messages


def getAllTopicsNames(cursor, print_out=False):
    """Returns all topics names."""
    topicNames = []
    # Get all records for 'topics'
    records = getAllElements(cursor, "topics", print_out=False)

    # Save all topics names
    for row in records:
        topicNames.append(row[1])  # 1 is for topic name TODO
    if print_out:
        print("\nTopics names are:")
        print(topicNames)

    return topicNames


def getAllMsgsTypes(cursor, print_out=False):
    """Returns all messages types."""
    msgsTypes = []
    # Get all records for 'topics'
    records = getAllElements(cursor, "topics", print_out=False)

    # Save all message types
    for row in records:
        msgsTypes.append(row[2])  # 2 is for message type TODO
    if print_out:
        print("\nMessages types are:")
        print(msgsTypes)

    return msgsTypes


def getMsgType(cursor, topic_name, print_out=False):
    """Returns the message type of that specific topic."""
    msg_type = []
    # Get all topics names and all message types
    topic_names = getAllTopicsNames(cursor, print_out=False)
    msgs_types = getAllMsgsTypes(cursor, print_out=False)

    # look for topic at the topic_names list, and find its index
    for index, element in enumerate(topic_names):
        if element == topic_name:
            msg_type = msgs_types[index]
    if print_out:
        print("\nMessage type in", topic_name, "is", msg_type)

    return msg_type


def read_msg_recursive(m):
    _list = []

    if "get_fields_and_field_types" in dir(m):
        fields_and_field_types = m.get_fields_and_field_types()
        for key in fields_and_field_types.keys():
            _list += read_msg_recursive(getattr(m, key))
    else:
        if type(m) == np.ndarray:
            m = m.tolist()
            return m
        elif type(m) == list:
            return m
        elif type(m) == array.array:
            return list(m)
        else:
            return [m]

    return _list


def convert_rosbag_to_csv(bag_file, csv_dir_path):
    # _topic_name = topic_name.replace('/', '_') + '.csv'

    # if _topic_name[0] == "_":
    #    _topic_name = _topic_name[1:]

    ### connect to the database
    conn, c = connect(bag_file)

    ### get all topics names and types
    topic_names = getAllTopicsNames(c, print_out=False)
    topic_types = getAllMsgsTypes(c, print_out=False)

    num_of_topics = len(topic_names)

    # Create a map for quicker lookup
    # type_map = {topic_names[i]:topic_types[i] for i in range(len(topic_types))}

    ### get all timestamps and all messages
    # t, msgs = getAllMessagesInTopic(c, topic_name, print_out=True)

    # Deserialize the message
    # msg_type = get_message(type_map[topic_name])  # Assuming type_map is a dictionary mapping topic names to message types

    # Open the CSV file for writing
    for i in range(num_of_topics):

        topic_name = topic_names[i]
        topic_type = topic_types[i]
        _, msgs = getAllMessagesInTopic(c, topic_name, print_out=True)
        msg_type = get_message(topic_type)

        _topic_name = topic_name.replace("/", "_") + ".csv"

        if _topic_name[0] == "_":
            _topic_name = _topic_name[1:]

        csv_file_path = csv_dir_path + "/" + _topic_name  # topic_name.replace('/', '_') + '.csv'

        # print( msg_type )

        with open(csv_file_path, "w", newline="") as csvfile:
            # Create a CSV writer
            csv_writer = csv.writer(csvfile)

            for message in msgs:
                deserialized_msg = deserialize_message(message, msg_type)
                csv_writer.writerow(read_msg_recursive(deserialized_msg))

    ### close connection to the database
    close(conn)


def rosbag_to_csv(bag_file_path, csv_dir_path=None):

    # for topic_name in topic_name_list:
    if csv_dir_path is None:
        csv_dir_path = bag_file_path

    if not os.path.isdir(csv_dir_path):
        os.mkdir(csv_dir_path)

    # print( csv_dir_path )
    bag_files = glob.glob(bag_file_path + "/*.db3")
    if len(bag_files) == 0:
        print("No bag files found in ", bag_file_path)
        return
    elif len(bag_files) > 1:
        print("Multiple bag files found in ", bag_file_path)
        return
    bag_db_file_path = bag_files[0]
    # bag_db_file_path =  bag_file_path + "/" + bag_file_path.split("/")[-1] + "_0.db3"
    # print("bag_db_file_path : ", bag_db_file_path)
    convert_rosbag_to_csv(bag_db_file_path, csv_dir_path)

    print("csv files are saved in ", csv_dir_path)


if __name__ == "__main__":
    # python3 rosbag_to_csv.py --ag_dir=
    parser = argparse.ArgumentParser()
    parser.add_argument("--bag_dir")
    args = parser.parse_args()

    bag_file_path = args.bag_dir

    rosbag_to_csv(bag_file_path)
