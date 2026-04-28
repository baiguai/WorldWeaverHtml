#!/usr/bin/env python3
import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext, Menu
import re

class DataLoader:
    def __init__(self, root):
        self.root = root
        self.root.title("WorldWeaver DataLoader")
        self.root.geometry("1200x800")
        self.root.config(bg='black')
        self.current_file = None
        self.setup_ui()

    def setup_ui(self):
        btn_frame = tk.Frame(self.root, bg='black')
        btn_frame.pack(fill=tk.X, padx=5, pady=5)

        tk.Button(btn_frame, text="Open (Ctrl+O)", command=self.open_file, width=15,
                  bg='#333', fg='white', activebackground='#555').pack(side=tk.LEFT, padx=2)
        tk.Button(btn_frame, text="Select DB (Ctrl+D)", command=self.select_database, width=15,
                  bg='#333', fg='white', activebackground='#555').pack(side=tk.LEFT, padx=2)
        tk.Button(btn_frame, text="Save (Ctrl+S)", command=self.save_file, width=15,
                  bg='#333', fg='white', activebackground='#555').pack(side=tk.LEFT, padx=2)
        tk.Button(btn_frame, text="Exit (Ctrl+Q)", command=self.root.destroy, width=15,
                  bg='#333', fg='white', activebackground='#555').pack(side=tk.LEFT, padx=2)

        self.text = scrolledtext.ScrolledText(self.root, wrap=tk.NONE, font=("Consolas", 10))
        self.text.config(bg='black', fg='white', insertbackground='white',
                        selectbackground='#555', selectforeground='white')
        self.text.pack(fill=tk.BOTH, expand=True, padx=5, pady=(0,5))

        self.root.bind('<Control-o>', lambda e: self.open_file())
        self.root.bind('<Control-d>', lambda e: self.select_database())
        self.root.bind('<Control-s>', lambda e: self.save_file())
        self.root.bind('<Control-q>', lambda e: self.root.destroy())
        self.root.bind('<Control-v>', self.manual_paste)
        self.root.bind('<Control-V>', self.manual_paste)

        # Right-click menu for paste
        self.context_menu = Menu(self.root, tearoff=0, bg='#333', fg='white',
                                 activebackground='#555', activeforeground='white')
        self.context_menu.add_command(label="Paste", command=self.manual_paste)

        def show_menu(event):
            self.context_menu.post(event.x_root, event.y_root)
        self.text.bind('<Button-3>', show_menu)

    def manual_paste(self, event=None):
        try:
            text = self.root.clipboard_get()
            if text:
                self.text.insert(tk.INSERT, text)
                return 'break'
        except tk.TclError:
            pass

        # Fallback for Linux: try xclip or xsel for large clipboard content
        import subprocess
        for cmd in [['xclip', '-selection', 'clipboard', '-o'],
                    ['xsel', '--clipboard', '--output']]:
            try:
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
                if result.returncode == 0 and result.stdout:
                    self.text.insert(tk.INSERT, result.stdout)
                    return 'break'
            except:
                continue
        return 'break'

    def open_file(self):
        file = filedialog.askopenfilename(
            title="Select HTML file",
            filetypes=[("HTML files", "*.html"), ("All files", "*.*")]
        )
        if file:
            self.current_file = file
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    content = f.read()
                self.text.delete(1.0, tk.END)
                self.text.insert(1.0, content)
                self.root.title(f"DataLoader - {file}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open file: {e}")

    def select_database(self):
        content = self.text.get(1.0, tk.END)

        # Find start of database (including 'let database = [')
        start_match = re.search(r'(let\s+database\s*=\s*\[)', content, re.IGNORECASE)
        if not start_match:
            messagebox.showwarning("Not Found", "Could not find 'let database = [' in the file.")
            return

        start_pos = start_match.start(1)

        # Find the opening '[' position
        open_bracket_pos = start_match.start(1) + start_match.group(1).rfind('[')

        # Find matching closing ']'
        bracket_count = 1
        end_pos = open_bracket_pos + 1
        while end_pos < len(content):
            char = content[end_pos]
            if char == '[':
                bracket_count += 1
            elif char == ']':
                bracket_count -= 1
                if bracket_count == 0:
                    end_pos += 1  # Include the closing ']'
                    break
            end_pos += 1

        if bracket_count != 0:
            messagebox.showwarning("Not Found", "Could not find the end of the database array.")
            return

        # Now find the semicolon after the closing ']'
        semi_pos = end_pos
        while semi_pos < len(content):
            if content[semi_pos] == ';':
                end_pos = semi_pos + 1  # Include the semicolon
                break
            if not content[semi_pos].isspace():
                break
            semi_pos += 1

        # Convert character positions to text widget indices
        def offset_to_index(offset):
            lines = content.split('\n')
            current = 0
            for i, line in enumerate(lines):
                if offset <= current + len(line):
                    return f"{i + 1}.{offset - current}"
                current += len(line) + 1
            return f"{len(lines)}.end"

        start_index = offset_to_index(start_pos)
        end_index = offset_to_index(end_pos)

        # Select the database in the text widget
        self.text.tag_remove(tk.SEL, 1.0, tk.END)
        self.text.tag_add(tk.SEL, start_index, end_index)
        self.text.see(start_index)
        self.text.focus()

    def save_file(self):
        if not self.current_file:
            messagebox.showwarning("No File", "No file is open.")
            return
        try:
            content = self.text.get(1.0, tk.END)
            with open(self.current_file, 'w', encoding='utf-8') as f:
                f.write(content)
            messagebox.showinfo("Saved", f"File saved successfully:\n{self.current_file}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save file: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = DataLoader(root)
    root.mainloop()
