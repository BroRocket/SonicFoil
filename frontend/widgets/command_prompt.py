from customtkinter import *

class CTkCommand(CTkTextbox):
    def __init__(self, master, directory_name="SonicFoil", **kwargs):
        super().__init__(master, wrap='char', **kwargs)

        self.directory = "<" + directory_name + "> "

        self.insert('end', "Program Start....\n" + self.directory)

        self.history = []
        self.history_index = -1

        self.accepting_input = True

        self.bind("<Return>", self.on_enter)
        self.bind("<BackSpace>", self.on_backspace)
        self.bind("<Key>", self.on_key)
        self.bind("<Up>", self.history_up)
        self.bind("<Down>", self.history_down)
        self.bind("<Left>", self.on_left)

        
    def _switch_state(self) -> None:
        if self.accepting_input is True:
            self.accepting_input = False
        else:
            self.accepting_input = True
            
    def show_message(self, message: str, last_msg : bool) -> None:
        self._switch_state()
        self.insert('end', f"\n{message}")
        if last_msg is True:
            self.insert('end', "\n" + self.directory)
            self._switch_state()
        self.see('end')

    def on_enter(self, event):
        if not self.accepting_input:
            return "break"
    
        self._switch_state()
        line = self.get_current_command()

        # Prevent newline insertion
        self.history.append(line)
        self.history_index = len(self.history)

        output = self.execute_command(line)

        if output:
            self.insert("end", "\n" + output)

        self.insert("end", "\n" + self.directory)
        self.see("end")
        self._switch_state()

        return "break"

    def execute_command(self, cmd: str):
        """
        Override or extend this method to add new commands.
        """
        cmd = cmd.strip()

        if cmd == "":
            return ""

        elif cmd == "help":
            return "Available commands:\n help \n echo <text> \n save <> \n clear \n exit/quit"

        elif cmd.startswith("echo "):
            return cmd[5:]

        elif cmd == "clear":
            self.clear_screen()
            return ""
        
        elif cmd.startswith("Airfoil Editor"):
            self.master.master.master.airfoil_editor_page()
            return ""
        
        elif cmd == "exit" or cmd == "quit":
            self.quit()
            return ""
        
        elif cmd.startswith("save "):
            cmd = cmd[5:]
            inputs = cmd[5:].split()

            selected_configs = {"methods": [], "coef": []}
            allowed_configs = {"methods": ["wave", "ackeret", "friction", "combined", "inviscid", "viscid"], "coef": ["cl", "cd", "cl/cd", "cmle"]}
            for i, input in enumerate(inputs):
                if input == "-methods":
                    j = i + 1
                    method = inputs[j].lower()
                    while len(inputs) < j and method[0] != "-":
                        if method in allowed_configs["methods"]:
                            selected_configs["methods"].append(method)
                        else:
                            self.show_message(f"Method '{inputs[j]}' not recognized. Please choose from the following methods: {selected_configs['methods']}", True)
                        j += 1
                        method = inputs[j].lower()
                elif input == "-coef":
                    j = i + 1
                    coeficient = inputs[j].lower()
                    while len(inputs) < j and coeficient[0] != "-":
                        if coeficient in allowed_configs["coef"]:
                            selected_configs["coef"].append(coeficient)
                        else:
                            self.show_message(f"Coefficient '{inputs[j]}' not recognized. Please choose from the following coeffecients: {selected_configs['coef']}", True)
                        j += 1
                        coeficient = inputs[j].lower()

            if len(selected_configs["methods"]) == 0:
                selected_configs["methods"] = allowed_configs["methods"]
            if len(selected_configs["coef"]) == 0:
                selected_configs["coef"] = allowed_configs["coef"]
       
            if cmd.startswith("data"):
               self.master.master._save_data(selected_configs)

            elif cmd.startswith("plot"):
                self.master.master._save_plots(selected_configs)
                
            return ""


        return f"Unknown command: {cmd}"

    # ===============================
    # Helpers
    # ===============================
    def get_current_command(self):
        # Find text after the last prompt
        text = self.get("end-1l", "end").strip()
        if text.startswith(self.directory):
            return text[len(self.directory):]
        return text

    def clear_screen(self):
        self.delete("1.0", "end")
        self.insert("end", self.directory)

    # ===============================
    # Prevent Editing Before Prompt
    # ===============================
    def on_backspace(self, event):
        if not self.accepting_input:
            return "break"
        # Don't allow deleting the prompt
        cursor = self.index("insert")
        prompt_index = self.index("end-1l+%dc" % len(self.directory))

        if self.compare(cursor, "<=", prompt_index):
            return "break"  # block backspace

    def on_key(self, event):
        if not self.accepting_input:
            return "break"
        elif event.keysym in ("Return", "BackSpace", "Up", "Down"):
            return

        cursor = self.index("insert")
        prompt_index = self.index("end-1l+%dc" % len(self.directory))

        # Prevent typing before the prompt
        if self.compare(cursor, "<=", prompt_index):
            self.mark_set("insert", "end")
        self.see("end")

    def on_left(self, event):
        if not self.accepting_input:
            return "break"

        prompt_index = self.index("end-1l+%dc" % len(self.directory))
        cursor = self.index("insert")

        # If cursor tries to move into or before the prompt — block it
        if self.compare(cursor, "<=", prompt_index):
            self.mark_set("insert", "end")
            return "break"

        # Otherwise allow left arrow normally
        return

    # ===============================
    # Command History Navigation
    # ===============================
    def history_up(self, event):
        if not self.accepting_input:
            return "break"
        elif not self.history:
            return "break"

        self.history_index = max(0, self.history_index - 1)
        self.replace_current_line(self.history[self.history_index])
        return "break"

    def history_down(self, event):
        if not self.accepting_input:
            return "break"
        elif not self.history:
            return "break"

        self.history_index = min(len(self.history), self.history_index + 1)

        if self.history_index == len(self.history):
            self.replace_current_line("")
        else:
            self.replace_current_line(self.history[self.history_index])

        return "break"

    def replace_current_line(self, text):
        self.delete("end-1l", "end")
        self.insert("end", "\n" + self.directory + text)
        self.mark_set("insert", "end")

        
