#!/usr/bin/env python
# Copyright (C) 2005-2025, UNIGINE. All rights reserved.
from ftplib import FTP
import os
import sys
import zipfile


def download_file_from_ftp(host, username, password, remote_file, local_path):
    remote_path = os.path.dirname(remote_file)
    try:
        ftp = FTP(host)
        ftp.login(username, password)
        print(f"Successfully connected to FTP server {host} as {username}")

        # change ftp directory
        parts = [d for d in remote_path.split(os.sep) if d]
        for part in parts:
            try:
                ftp.cwd(part)
                print(f"Changed to directory {part}")
            except:
                print(f"Directory {part} does exists")
                return False

        # Get filename from path
        filename = os.path.basename(remote_file)
        local_filepath = os.path.join(local_path, filename)

        # Download file in binary mode
        with open(local_filepath, 'wb') as file:
            print(f"Downloading {filename} ...")
            ftp.retrbinary(f"RETR {filename}", file.write)
            print(f"File {filename} successfully downloaded to {local_filepath}")

        # Close connection
        ftp.quit()
        return True

    except Exception as e:
        print(f"Error downloading file from FTP: {e}")
        return False


def extract_archive(archive_path):
    """Extract zip archive and remove it"""
    extract_to = os.path.abspath(os.path.dirname(__file__))
    showcase_dir = os.path.join(extract_to, "data", "showcase_content")
    try:
        # Extract archive
        print(f"Extracting {archive_path} to data...")
        with zipfile.ZipFile(archive_path, 'r') as zip_ref:
            zip_ref.extractall(extract_to)
            extracted = zip_ref.namelist()
        print(f"Extracted {len(extracted)} items to {showcase_dir}")

        # Remove archive if requested
        os.remove(archive_path)
        print(f"Removed archive: {archive_path}")

        return extracted

    except Exception as e:
        print(f"Extraction error: {e}")
        return None


if __name__ == "__main__":
    FTP_HOST = "files.unigine.com"
    FTP_USER = "samples_r"
    FTP_PASS = "VbHU9vk0uCxyPmTr"
    
    # File paths
    SDK_VERSION = "2.20"
    ZIP_FILE_NAME = f"cpp_samples_{SDK_VERSION}_showcase_content.zip"
    REMOTE_FILE_PATH = os.path.join("samples", f"release_{SDK_VERSION}", ZIP_FILE_NAME)
    LOCAL_DIR = os.path.abspath(os.path.dirname(__file__))
    LOCAL_FILE_PATH = os.path.join(LOCAL_DIR, ZIP_FILE_NAME)

    # Check if archive exists - remove first
    if os.path.isfile(LOCAL_FILE_PATH):
        os.remove(LOCAL_FILE_PATH)

    # Download and unpack
    if download_file_from_ftp(FTP_HOST, FTP_USER, FTP_PASS, REMOTE_FILE_PATH, LOCAL_DIR):
        if extract_archive(LOCAL_FILE_PATH) is None:
            print("Failed to extract files from archive")
            sys.exit(1)

        print("Completed successfully")
