#!/usr/local/bin/python3

from setuptools import setup

setup(
    name="bang_question",
    version="0.1",
    description="Proof-of-concept cyber investigative framework",
    author="Joe St Sauver",
    author_email="stsauver@fsi.io",
    scripts=['bin/bang_question'],
    install_requires=[
        'dnspython',
        'Image',
        'ipwhois',
        'ip2location',
        'kaleido',
        'lxml',
        'numpy',
        'pandas',
        'Pillow',
        'plotly-express',
        'pyasn',
        'pycurl',
        'PyMuPDF',
        'pyppeteer2',
        'python-igraph',
        'requests',
        'whois',
    ],
    license="Apache Software License",
    packages=setuptools.find_packages(),
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python :: 3.9",
    ],
    python_requires='>=3.9.1',
)

