# DS-Mini-Project

# 📚 Python E-Book Reader (Data Structure Focus)

## 🌟 Project Summary

This project is a Graphical User Interface (GUI) E-Book Reader built in Python using `tkinter`. Its primary function is to demonstrate the practical application of core data structures, specifically **Stacks** and **Lists**, to manage a user's reading experience and navigation history.

---

## ⚙️ Data Structures in Action

The design relies on efficient data structure manipulation to handle core reader functionality:

* **Page Content (List/Array):** The entire book content is segmented and stored as a simple **List** in memory, where each element represents one page of text.
* **Navigation History (Stacks):**
    * A **Back Stack** stores the index of every page visited, enabling the **"Previous"** action (pop from the stack).
    * A separate **Forward Stack** stores pages undone by the "Previous" action, enabling the **"Forward"** action (pop from this stack).
* **Persistent State (JSON):** The user's last-read page number and defined bookmarks are saved to a `.json` file for persistence, allowing the reader to resume from the last session.

---

## 🚀 Key Features

* **GUI Interface:** Simple, easy-to-use interface for displaying text and controls.
* **Dynamic Navigation:** Seamless "Previous" and "Forward" navigation powered by Stacks.
* **File Support:** Built to load content from `.txt` files (with optional support for `.pdf` via `PyMuPDF`).
* **Session Persistence:** Saves and loads reading progress and bookmarks.

---

## 🛠️ Getting Started

1.  **Prerequisites:** Requires Python 3.x.
2.  **Dependencies:** (Optional) Install `PyMuPDF` for PDF support: `pip install PyMuPDF`
3.  **Run:** Execute the Python file and load a book file (`.txt` recommended).
