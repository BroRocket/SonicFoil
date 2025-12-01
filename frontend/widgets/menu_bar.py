import customtkinter as ctk

class CTkMenuBar(ctk.CTkFrame):
    def __init__(self, master, menus: dict, **kwargs):
        super().__init__(master, height=28, fg_color=("gray85", "gray20"), **kwargs)

        bg = ("gray85", "gray20")  # background color for menu items

        self.grid_columnconfigure(tuple(range(len(menus))), weight=0)

        for i, (menu_name, commands) in enumerate(menus.items()):
            menu_button = ctk.CTkOptionMenu(
                self,
                values=list(commands.keys()),
                command=lambda choice, cmds=commands: cmds[choice](),
                fg_color=bg,
                button_color=bg,
                button_hover_color=("gray75", "gray25"),
                dropdown_fg_color=("gray90", "gray15"),
                dropdown_hover_color=("gray80", "gray25"),
                anchor="w",
                width=100
            )

            # show the menu label
            menu_button.set(menu_name)

            menu_button.grid(row=0, column=i, padx=6, pady=2)

