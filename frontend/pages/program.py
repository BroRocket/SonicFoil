
import os
import math
import csv
from customtkinter import *
from tkinter import Menu, filedialog, StringVar
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from frontend.utilities import ui_messages
from frontend.widgets.plot import CTkPlot
from frontend.widgets.command_prompt import CTkCommand
import sonicfoil_backend as SonicFoil



def new_solver_page():
    window = CTkToplevel()
    window.title("SonicFoil")
    page = ProgramPage(window)
    page.pack(padx=5, pady=5, fill="both", expand=True)

def open_airfoil():
    filename = filedialog.askopenfilename(initialdir = os.path.join(os.getcwd(), "Airfoils"), title = "Select a File", filetypes = (("Airfoils", "*.dat*"),("all files", "*.*")))
    window = CTkToplevel()
    page = ProgramPage(window)
    page.pack(padx=5, pady=5, fill="both", expand=True) 
    page.load_airfoil_file_opt.set(filename)
    page._load_airfoil()


#self.master is root!
#ADD XCP AND YCP PLOTS

class ProgramPage(CTkFrame):
    def __init__(self, master: CTk, **kwargs):
        super().__init__(master, **kwargs)

        self.Airfoils = []
        self.Solvers = []

        self.grid_columnconfigure((0 , 1 , 2, 3, 4), weight=1)
        self.grid_rowconfigure((0, 1, 2, 3, 4, 5, 6), weight=1)
        
        #Configure bar
        self.menu = Menu(self.master)
        self.file_menu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='File', menu=self.file_menu)
        self.file_menu.add_command(label='New', command=new_solver_page) 
        self.file_menu.add_command(label='Open...', command=open_airfoil) 
        self.file_menu.add_command(label='Airfoil Editor', command=self.master.airfoil_editor_page)
        self.file_menu.add_separator()
        self.file_menu.add_command(label='Settings', command=None) # add command
        self.file_menu.add_command(label='Exit', command=self.master.quit)
        helpmenu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='Help', menu=helpmenu)
        helpmenu.add_command(label='About', command=None) # add command
        helpmenu.add_command(label='Guide', command=None) # add command

        self.master.configure(menu=self.menu)

        ##########################################
        #load airfoil frame
        self.load_airfoil_frm = CTkFrame(self)
        self.load_airfoil_frm.grid_columnconfigure((0 , 1 , 2, 3), weight=1)
        self.load_airfoil_frm.grid_rowconfigure((0, 1, 2), weight=1)

        self.load_airfoil_title_lbl = CTkLabel(self.load_airfoil_frm, text="Load Airfoil", font=("Arial", 16), anchor="center")
        self.load_airfoil_file_lbl = CTkLabel(self.load_airfoil_frm, text="File:", font=("Arial", 12), anchor="e")
        self.load_airfoil_file_opt = CTkOptionMenu(self.load_airfoil_frm, font=("Arial", 12), values= ["..."] + self._get_available_projects())
        self.load_airfoil_browse_file_btn = CTkButton(self.load_airfoil_frm, text="Browse", font=("Arial", 12), command=self._browse_files, anchor="w")
        self.load_airfoil_load_btn = CTkButton(self.load_airfoil_frm, text="Load Airfoil", font=("Arial", 14), command=self._load_airfoil)
        self.clear_airfoild_btn = CTkButton(self.load_airfoil_frm, text="Clear Airfoils", font=("Arial", 14), command=self._clear_airfoils)


        self.load_airfoil_title_lbl.grid(row=0, column=0, columnspan=4, padx=5, pady=5, sticky="nsew")
        self.load_airfoil_file_lbl.grid(row=1, column=0, padx=(5, 1), pady=5, sticky="ew")
        self.load_airfoil_file_opt.grid(row=1, column=1, columnspan = 2, padx=5, pady=5, sticky = "ew")
        self.load_airfoil_browse_file_btn.grid(row=1, column=3, padx=(1, 5), pady=5, sticky="w")
        self.load_airfoil_load_btn.grid(row=2, column=0, columnspan = 2, padx=5, pady=5, sticky = "ew")
        self.clear_airfoild_btn.grid(row=2, column=2, columnspan = 2, padx=5, pady=5, sticky = "ew")

        self.load_airfoil_frm.grid(row=0, rowspan=2, column=0, columnspan=2, padx=5, pady=5, sticky="nsew")

        ###########################
        # input frame
        self.inputs_frm = CTkFrame(self)
        self.inputs_frm.grid_columnconfigure((0, 1, 2, 3), weight=1)
        self.inputs_frm.grid_rowconfigure((0, 1, 2, 3, 4), weight=1)

        self.input_lbl = CTkLabel(self.inputs_frm, text="Solver Inputs", font=("Arial", 16), anchor="center")
        self.input_lbl.grid(row=0, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

        self.flow_conditions_input_frm = CTkFrame(self.inputs_frm)
        self.flow_conditions_input_frm.grid_columnconfigure((0, 1, 2, 3) , weight=1)
        self.flow_conditions_input_frm.grid_rowconfigure((0, 1, 2), weight=1)
        self.freestream_lbl = CTkLabel(self.flow_conditions_input_frm, text="Freestream Flow Conditions", font=("Arial", 14), anchor="center")
        self.mach_lbl = CTkLabel(self.flow_conditions_input_frm, text="Mach Number:", font=("Arial", 12), anchor="e")
        self.mach_entry_var = StringVar(value = "1.5")
        self.mach_entry_var.trace_add('write', self._render_methods)
        self.method_state = "sub"
        self.mach_entry = CTkEntry(self.flow_conditions_input_frm, textvariable=self.mach_entry_var)
        self.pressure_lbl = CTkLabel(self.flow_conditions_input_frm, text="Pressure (Pa):", font=("Arial", 12), anchor="e")
        self.pressure_entry = CTkEntry(self.flow_conditions_input_frm, placeholder_text="101325")
        self.density_lbl = CTkLabel(self.flow_conditions_input_frm, text="Density (kg/m^3):", font=("Arial", 12), anchor="e")
        self.density_entry = CTkEntry(self.flow_conditions_input_frm, placeholder_text="1.225") 
        self.temperature_lbl = CTkLabel(self.flow_conditions_input_frm, text="Temperature (K):", font=("Arial", 12), anchor="e")
        self.temperature_entry = CTkEntry(self.flow_conditions_input_frm, placeholder_text="273.15")

        self.freestream_lbl.grid(row=0, column=0, columnspan=4, padx=10, pady=(10, 5), sticky="nsew")
        self.mach_lbl.grid(row=1, column=0, padx=(10, 5), pady=(0, 5), sticky="nsew")
        self.mach_entry.grid(row=1, column=1, padx=(0, 5), pady=(0, 5), sticky="nsew")
        self.pressure_lbl.grid(row=1, column=2, padx=(5, 5), pady=(0, 5), sticky="nsew")
        self.pressure_entry.grid(row=1, column=3, padx=(0 ,10), pady=(0, 5), sticky="nsew")
        self.density_lbl.grid(row=2, column=0, padx=(10, 5), pady=(0, 10), sticky="nsew")
        self.density_entry.grid(row=2, column=1, padx=(0, 5), pady=(0, 10), sticky="nsew")
        self.temperature_lbl.grid(row=2, column=2, padx=(5, 5), pady=(0, 10), sticky="nsew")
        self.temperature_entry.grid(row=2, column=3, padx=(0 ,10), pady=(0, 10), sticky="nsew")
        self.flow_conditions_input_frm.grid(row=1, column=0, columnspan=4, padx=5, pady=5, sticky="nsew")

        # Make function that renders this based on mach number
        # call render fucntion
        self._render_methods()

        self.angles_frm = CTkFrame(self.inputs_frm)
        self.angles_frm.grid_rowconfigure((0, 1), weight=1)  # row with TabView
        self.angles_frm.grid_columnconfigure(0, weight=1)
        
        self.angles_lbl = CTkLabel(self.angles_frm, text="Angle of Attack", font=("Arial", 14), anchor="center")
        self.angles_tabs = CTkTabview(self.angles_frm, anchor="center")
        self.single_angle_tab = self.angles_tabs.add("Single")
        self.single_angle_tab.grid_columnconfigure((0, 1), weight=1)
        self.single_angle_tab.grid_rowconfigure(0, weight=1)
        self.single_angle_lbl = CTkLabel(self.single_angle_tab, text="AoA (degrees):", font=("Arial", 12), anchor="e")
        self.single_angle_entry = CTkEntry(self.single_angle_tab, placeholder_text="0")
        self.single_angle_lbl.grid(row=0, column=0, padx=(10, 5), pady=10, sticky="ew")
        self.single_angle_entry.grid(row=0, column=1, padx=(0, 10), pady=10, sticky="ew")

        self.angle_range_tab = self.angles_tabs.add("Range")
        self.angle_range_tab.grid_columnconfigure((0, 1, 2, 3), weight=1)
        self.angle_range_tab.grid_rowconfigure((0, 1), weight=1)
        self.angle_range_lbl = CTkLabel(self.angle_range_tab, text="AoA Interval (degrees):", font=("Arial", 12), anchor="e")
        self.angle_range_left_entry = CTkEntry(self.angle_range_tab, placeholder_text="0")
        self.angle_range_mid_lbl = CTkLabel(self.angle_range_tab, text="to", font=("Arial", 12), anchor="center")
        self.angle_range_right_entry = CTkEntry(self.angle_range_tab, placeholder_text="5")
        self.angle_range_step_size_lbl = CTkLabel(self.angle_range_tab, text="Step Size (degrees):", font=("Arial", 12), anchor="e")
        self.angle_range_step_size_entry = CTkEntry(self.angle_range_tab, placeholder_text="1")
        self.angle_range_lbl.grid(row=0, column=0, padx=(10, 5), pady=(10, 5), sticky="ew")
        self.angle_range_left_entry.grid(row=0, column=1, padx=0, pady=(10, 5), sticky="ew")
        self.angle_range_mid_lbl.grid(row=0, column=2, padx=5, pady=(10, 5), sticky="ew")
        self.angle_range_right_entry.grid(row=0, column=3, padx=(0, 10), pady=(10, 5), sticky="ew")
        self.angle_range_step_size_lbl.grid(row=1, column=0, padx=(10, 5), pady=(5, 10), sticky="ew")
        self.angle_range_step_size_entry.grid(row=1, column=1, padx=(0, 10), pady=(5, 10), sticky="ew")

        self.angles_lbl.grid(row=0, column=0, padx=10, pady=(10,5), sticky="ew")
        self.angles_tabs.grid(row=1, column=0, padx=10, pady=(5,10), sticky="nsew")
        self.angles_frm.grid(row=3, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

        self.solve_btn = CTkButton(self.inputs_frm, text="Run Solver", font=("Arial", 12), command=self._run, anchor="center")
        self.solve_btn.grid(row=4, column=1, columnspan=2, padx=10, pady=10, sticky="nsew")

        self.inputs_frm.grid(row=2, column=0, rowspan=5, columnspan=2, padx=5, pady=5, sticky="nsew")

        #############################3
        # plotter frame (use tabview)
        self.plots_frm = CTkFrame(self)
        self.plots_frm.grid_rowconfigure(0, weight=1)
        self.plots_frm.grid_columnconfigure(0, weight=1)

        self.plots = CTkTabview(self.plots_frm)

        self.airfoil_tab = self.plots.add("Airfoil")
        self.airfoil_plot = CTkPlot(self.airfoil_tab, "Airfoil Schematic", "x/c", "y/c", dpi=150)
        
        self.CL_tab = self.plots.add("CL")
        self.CL_plot = CTkPlot(self.CL_tab, "Coeffecient of Lift", "alpha (degrees)", "CL", dpi=150)

        self.Cd_tab = self.plots.add("Cd")
        self.Cd_plot = CTkPlot(self.Cd_tab, "Coeffecient of Drag", "alpha (degrees)", "Cd", dpi=150)

        self.CL_Cd_tab = self.plots.add("CL/Cd")
        self.CL_Cd_plot = CTkPlot(self.CL_Cd_tab, "Coeffecient of Lift/Drag Ratio", "alpha (degrees)", "CL/Cd", dpi=150)

        self.CLvCd_tab = self.plots.add("CLvsCd")
        self.CLvCd_plot = CTkPlot(self.CLvCd_tab, "Coeffecient of Lift vs. Coeffecient of Drag ", "Cd", "CL", dpi=150)

        self.C_MLE_tab = self.plots.add("C_MLE")
        self.C_MLE_plot = CTkPlot(self.C_MLE_tab, "Coefficient of Moment About Leading Edge", "alpha (degrees)", "C_MLE", dpi=150)

        self.airfoil_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.CL_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.Cd_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.CL_Cd_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.CLvCd_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.C_MLE_plot.pack(padx=0, pady=0, fill="both", expand=True)

        self.plots.grid(row=0, column=0, sticky="nsew")

        self.plots_frm.grid(row=0, column=2, rowspan=5, columnspan=3, padx=5, pady=5, sticky="nsew")

        ##### command interface frame
        self.text_output_frm = CTkFrame(self)

        self.command_interface = CTkCommand(self.text_output_frm)
        self.command_interface.pack(padx=10, pady=10, fill="both")

        self.text_output_frm.grid(row=5, rowspan=2, column=2, columnspan=3, padx=5, pady=5, sticky="nsew")


    def _render_methods(self, *args) -> None:  # need to  update varaibles elsewhere  
        try:
            mach = float(self.mach_entry.get())
        except:
            return

        if mach >= 1 and self.method_state != "sup":
            if hasattr(self, "method_frame") and self.method_frame is not None: 
                self.methods_frm.destroy()
            self.methods_frm = CTkFrame(self.inputs_frm)
            self.methods_frm.grid_columnconfigure((0, 1, 2), weight=1)
            self.methods_frm.grid_rowconfigure((0, 1), weight=1)
            self.method_lbl = CTkLabel(self.methods_frm, text="Supersonic Methods:", font=("Arial", 14), anchor="e")
            self.wave_var = StringVar(value="off")
            self.ackeret_var = StringVar(value="off")
            self.supersonic_friction_var = StringVar(value="off") # add a toggle for friction being selectable
            self.wave_method_radbtn = CTkCheckBox(self.methods_frm, text="Shock/Expansion", variable= self.wave_var, onvalue="on", offvalue="off", command=self._toggle_friciton_button)
            self.ackeret_method_radbtn = CTkCheckBox(self.methods_frm, text="Ackeret", command=None, variable= self.ackeret_var, onvalue="on", offvalue="off")
            self.supersonic_friction_switch = CTkSwitch(self.methods_frm, text="Skin Friction", command=None, variable= self.supersonic_friction_var, onvalue="on", offvalue="off", state="disabled")

            self.method_lbl.grid(row=0, column=0, padx=(10, 5), pady=5, sticky="nsew") 
            self.wave_method_radbtn.grid(row=0, column=1, padx=(5, 5), pady=5, sticky="nsew")
            self.ackeret_method_radbtn.grid(row=0, column=2, padx=(0, 5), pady=5, sticky="nsew")
            self.supersonic_friction_switch.grid(row=1, column=1, padx=(15, 10), pady=5, sticky="nsew")
            self.methods_frm.grid(row=2, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

            self.method_state = "sup"

        elif mach < 1 and self.method_state != "sub":
            if hasattr(self, "method_frame") and self.method_frame is not None: 
                self.methods_frm.destroy()
            self.methods_frm = CTkFrame(self.inputs_frm)
            self.methods_frm.grid_columnconfigure((0, 1, 2), weight=1)
            self.methods_frm.grid_rowconfigure(0, weight=1)
            self.method_lbl = CTkLabel(self.methods_frm, text="Subsonic Methods:", font=("Arial", 14), anchor="e")
            self.inviscid_var = StringVar(value="off")
            self.viscid_var = StringVar(value="off")
            self.subsonic_friction_var = StringVar(value="off")
            self.panel_method_radbtn = CTkCheckBox(self.methods_frm, text="Xfoil Inviscid", variable= self.inviscid_var, onvalue="on", offvalue="off")
            self.kutta_method_radbtn = CTkCheckBox(self.methods_frm, text="Xfoil Viscid", variable= self.viscid_var, onvalue="on", offvalue="off")

            self.method_lbl.grid(row=0, column=0, padx=(10, 5), pady=5, sticky="nsew") 
            self.panel_method_radbtn.grid(row=0, column=1, padx=(5, 5), pady=5, sticky="nsew")
            self.kutta_method_radbtn.grid(row=0, column=2, padx=(0, 5), pady=5, sticky="nsew")
            self.methods_frm.grid(row=2, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

            self.method_state = "sub"

    def _toggle_friciton_button(self):
        if self.method_state == "sup": # update state if on and disblae if off just diasbale
            wave_state = self.wave_var.get()
            if wave_state == "on":
                self.supersonic_friction_switch.configure(state="enabled")
            else:
                self.supersonic_friction_switch.configure(state="disabled")
                self.supersonic_friction_var.set("off") # check if changes

        elif self.method_state == "sub":
            pass


    def _get_available_projects(self) -> list:
        cur_dir = os.path.join(os.getcwd(), "Airfoils")
        if os.path.exists(cur_dir):
            available_files = []
            dir_items = os.scandir(cur_dir)
            for itm in dir_items:
                if os.path.splitext(itm.name)[1] == ".dat":
                    available_files.append(itm.name)
            return available_files if available_files else ["None"]
        else:
            os.makedirs(cur_dir)
        return ["None"]
    
    def _browse_files(self) -> None:
        filename = filedialog.askopenfilename(initialdir = os.path.join(os.getcwd(), "Airfoils"), title = "Select a File", filetypes = (("Airfoils", "*.dat*"),("all files", "*.*")))
        self.load_airfoil_file_opt.set(filename)
        
    def _load_airfoil(self) -> None:
        if len(self.Airfoils) == 4: # Arbritrary limit set based on basic matplotlib colors
            ui_messages.gui_error(f"Cannot load in another airfoil as limit of 7 airfoils has been reached")
            return

        filename = self.load_airfoil_file_opt.get()
        if os.path.exists(filename):
            if not filename.endswith(".dat"):
                ui_messages.gui_error(f'Entered filename "{filename}" is not the proper ".dat" format')
                return
            filepath = filename   
        elif os.path.exists(os.path.join(os.getcwd(), "Airfoils", filename)):
            filepath = os.path.join(os.getcwd(), "Airfoils", filename)
        else:
            ui_messages.gui_error(f'Entered filename "{filename}" does not exist')
            return
        
        try:
            airfoil = SonicFoil.Airfoil(filepath)
        except:
            ui_messages.gui_error(f'Aifoil File "{filename}" is not in correct format to be loaded in.')
            return

        if self.__check_airfoil_duplicate(airfoil) is False:
            self.Airfoils.append(airfoil)
            self.Solvers.append(SonicFoil.Solver(self.Airfoils[-1]))
            self._update_airfoil_plot()
            self.command_interface.show_message(f'Loaded in Airfoil: "{self.Airfoils[-1].name}" from file: {filepath}', True)
        else:
            self.command_interface.show_message(f"Airfoil with same name already loaded into the Solver. Check you haven't loaded in the same arifoil or rename the new airfoil.", True)

    def __check_airfoil_duplicate(self, new_airfoil) -> bool:
        for airfoil in self.Airfoils:
            if new_airfoil.name == airfoil.name:
                return True
        return False
        
    def _clear_airfoils(self) -> None:
        if len(self.Airfoils) > 0:
            self.Airfoils = []
            self.Solvers = []

            self.airfoil_plot.clear()
            self._clear_results_plots()
            self.command_interface.show_message("Airofils Cleared", True)

    def _update_airfoil_plot(self) -> None:
        linestyle_options = ["solid", "dotted", "dashed", "dashdot"]
        #self.airfoil_plot.clear()
        x = []
        y = []
        for segment in self.Airfoils[-1].top_segments:
            x.append(segment.start.x)
            y.append(segment.start.y)
        x.append(segment.end.x)
        y.append(segment.end.y)
        #   self.airfoil_plot.add_line(x, y, self.Airfoils[-1].name, color="k", linestyle=linestyle_options[len(self.Airfoils) - 1], marker=None)
        for segment in reversed(self.Airfoils[-1].bottom_segments):
            x.append(segment.end.x)
            y.append(segment.end.y)
        x.append(segment.start.x)
        y.append(segment.start.y)

        self.airfoil_plot.add_line(x, y, self.Airfoils[-1].name, color="k", linestyle=linestyle_options[len(self.Airfoils) - 1], marker=None)
        self.airfoil_plot.ax.set_aspect('equal', adjustable='datalim')
        self.airfoil_plot._refresh()

    def _clear_results_plots(self) -> None:
        self.CL_plot.clear()
        self.Cd_plot.clear()
        self.CL_Cd_plot.clear()
        self.CLvCd_plot.clear()
        self.C_MLE_plot.clear()
        self.command_interface.show_message("Result plots cleared", False)

    def _get_method(self) -> str:
        method = ""
        if self.method_state == "sup":
            if self.wave_var.get() == "on" and self.ackeret_var.get() == "on":
                method += "b"
            elif self.wave_var.get() == "on":
                method += "w"
            elif self.ackeret_var.get() == "on":
                method += "a"
            else:
                ui_messages.gui_error("Please select a solver method to solve the pressure distribution, cannot run without a selection.")
                return ""
            if self.supersonic_friction_var.get() == "on":
                if method == "a":
                    ui_messages.gui_popup("Cannot solve skin friciton using ackeret method results. No friction result will be produced.")
                else:
                    method += "d"
                
        elif self.method_state == "sub":
            if self.inviscid_var.get() == "on" and self.viscid_var.get() == "on":
                method += "b"
            elif self.inviscid_var.get() == "on":
                method += "i"
            elif self.viscid_var.get() == "on":
                method += "v"
            else:
                ui_messages.gui_error("Please select a solver method to solve the pressure distribution, cannot run without a selection.")
                return ""

        return method

    def _run(self) -> None:
        self.command_interface.show_message("Starting Task...", False)
        method = self._get_method()
        if method == "": 
            return

        mach = float(self.mach_entry.get()) if self.mach_entry.get() != '' else 1.0
        pressure = float(self.pressure_entry.get()) if self.pressure_entry.get() != '' else 101325
        density = float(self.density_entry.get()) if self.density_entry.get() != '' else 1.225
        temperature = float(self.temperature_entry.get()) if self.temperature_entry.get() != '' else 273.15

        if mach > 5:
            ui_messages.gui_error("Mach number is out of allowbale regime. Mach Number must be within M > 5")
            return
        elif pressure <= 0:
            ui_messages.gui_error("Pressure cannot be below zero")
            return
        elif density <= 0:
            ui_messages.gui_error("Density must be greater than zero")
            return
        elif temperature < 0:
            ui_messages.gui_error("Temperature in Kelvin cannot be less than zero")
            return

        active_tab = self.angles_tabs.get()
        if active_tab == "Single":

            AoA = [float(self.single_angle_entry.get()) * math.pi / 180] if self.single_angle_entry.get() != '' else [0]

        else:
            start = float(self.angle_range_left_entry.get()) * math.pi / 180 if self.angle_range_left_entry.get() != '' else 0
            end = float(self.angle_range_right_entry.get()) * math.pi / 180 if self.angle_range_right_entry.get() != '' else 5 * math.pi / 180
            step = float(self.angle_range_step_size_entry.get()) * math.pi / 180 if self.angle_range_step_size_entry.get() != '' else 1 * math.pi / 180

            if start > end:
                ui_messages.gui_error("Start angle cannot be greater than end angle")
                return
            elif step > end - start:
                ui_messages.gui_error("Step size cannot be greater than difference of start and end")
                return

            AoA = []
            a = start
            while a <= end + 1e-12:
                AoA.append(a)
                a += step
            AoA = [round(angle, 8) for angle in AoA]

        if len(self.Solvers) < 1:
            ui_messages.gui_error("Please load in an Airfoil to use the solver")
            return

        self._clear_results_plots()

        for i, solver in enumerate(self.Solvers):
            solver.solve_range(method, AoA, mach, pressure, temperature, density)

            if solver.success is True:
                self.command_interface.show_message(f"Solver Completed Running for Airofil '{self.Airfoils[i].name}'", False)
                self._update_result_plots(solver, AoA, i)
            else:
                self.command_interface.show_message(f"Solver encountered following error on Airfoil '{self.Airfoils[i].name}':\n{solver.error_msg}\n Computed values up to error will be plotted", False)
                self._update_result_plots(solver, AoA[:len(solver.Results)], i)
        
        self.command_interface.show_message("Task Complete\n", True)

    def _update_result_plots(self, solver, AoA: list, solver_index: int) -> None:

        self.Results = []
        linestyle_options = ["solid", "dotted", "dashed", "dashdot"]

        Cl = {"wave": [], "ackeret": [], "supersonic_friction": [], "supersonic_combined": [], "inviscid": [], "viscid": []}
        Cd = {"wave": [], "ackeret": [], "supersonic_friction": [], "supersonic_combined": [],  "viscid": []}
        CL_Cd = {"wave": [], "ackeret": [], "supersonic_friction": [], "supersonic_combined": [], "viscid": []}
        C_Mle = {"wave": [], "ackeret": [], "inviscid": [], "viscid": []} # curently no Cmle for friction
        #currently no xcp and ycp plottingg

        #update this
        for result in solver.Results: 
            wave = result.wave_solution
            ackeret = result.ackeret_solution
            supersonic_friction = result.skin_friction_solution_supersonic 
            inviscid = result.xfoil_invicid_solution
            viscid = result.xfoil_viscid_solution
            if wave is not None:
                Cl["wave"].append(wave.Forces.CL)
                Cd["wave"].append(wave.Forces.Cd)
                CL_Cd["wave"].append(wave.Forces.CL_Cd)
                C_Mle["wave"].append(wave.Forces.C_MLE)
            if ackeret is not None:
                Cl["ackeret"].append(ackeret.Forces.CL)
                Cd["ackeret"].append(ackeret.Forces.Cd)
                CL_Cd["ackeret"].append(ackeret.Forces.CL_Cd)
                C_Mle["ackeret"].append(ackeret.Forces.C_MLE)
            if supersonic_friction is not None:
                Cl["supersonic_friction"].append(supersonic_friction.Forces.CL)
                Cd["supersonic_friction"].append(supersonic_friction.Forces.Cd)
                CL_Cd["supersonic_friction"].append(supersonic_friction.Forces.CL_Cd)
            if inviscid is not None:
                Cl["inviscid"].append(inviscid.Forces.CL)
                C_Mle["inviscid"].append(inviscid.Forces.C_MLE)
            if viscid is not None:
                Cl["viscid"].append(viscid.Forces.CL)
                Cd["viscid"].append(viscid.Forces.Cd)
                CL_Cd["viscid"].append(viscid.Forces.CL_Cd)
                C_Mle["viscid"].append(viscid.Forces.C_MLE)
            

        
        if Cl["wave"] != [] and Cl["supersonic_friction"] != []:
            for i in range(len(Cl["wave"])):
                Cl["supersonic_combined"].append(Cl["wave"][i]+Cl["supersonic_friction"][i])
                Cd["supersonic_combined"].append(Cd["wave"][i]+Cd["supersonic_friction"][i])
                CL_Cd["supersonic_combined"] = [combined_cl / combined_cd for combined_cl, combined_cd in zip(Cl["supersonic_combined"], Cd["supersonic_combined"])]

        angles = [rad * 180 / math.pi for rad in AoA]

        self.Results.append({"airfoil": self.Airfoils[solver_index].name, "AoA": angles, "CL": Cl, "Cd": Cd, "CL_Cd": CL_Cd, "C_MLE": C_Mle})

        if self.angles_tabs.get() == "Single":
            if Cl["wave"] != []:
                self.CL_plot.add_point(angles, Cl["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_point(angles, Cd["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_point(angles, CL_Cd["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_point(Cd["wave"], Cl["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_point(angles, C_Mle["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
            if Cl["ackeret"] != []:
                self.CL_plot.add_point(angles, Cl["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_point(angles, Cd["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_point(angles, CL_Cd["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_point(Cd["ackeret"], Cl["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_point(angles, C_Mle["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
            if Cl["supersonic_friction"] != []:
                self.CL_plot.add_point(angles, Cl["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_point(angles, Cd["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_point(angles, CL_Cd["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_point(Cd["supersonic_friction"], Cl["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
            if Cl["supersonic_combined"] != []:
                self.CL_plot.add_point(angles, Cl["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_point(angles, Cd["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_point(angles, CL_Cd["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_point(Cd["supersonic_combined"], Cl["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
            if Cl["inviscid"] != []:
                self.CL_plot.add_point(angles, Cl["inviscid"], "Xfoil Inviscid", color="r", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_point(angles, C_Mle["inviscid"], "viscid", color="r", linestyle=linestyle_options[solver_index])
            if Cl["viscid"] != []:
                self.CL_plot.add_point(angles, Cl["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_point(angles, Cd["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_point(angles, CL_Cd["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_point(Cd["viscid"], Cl["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_point(angles, C_Mle["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
        
        else:
            if Cl["wave"] != []:
                self.CL_plot.add_line(angles, Cl["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_line(angles, Cd["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_line(angles, CL_Cd["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_line(Cd["wave"], Cl["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_line(angles, C_Mle["wave"], "Shock/Expansion", color="r", linestyle=linestyle_options[solver_index])
            if Cl["ackeret"] != []:
                self.CL_plot.add_line(angles, Cl["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_line(angles, Cd["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_line(angles, CL_Cd["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_line(Cd["ackeret"], Cl["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_line(angles, C_Mle["ackeret"], "Ackeret", color="b", linestyle=linestyle_options[solver_index])
            if Cl["supersonic_friction"] != []:
                self.CL_plot.add_line(angles, Cl["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_line(angles, Cd["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_line(angles, CL_Cd["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_line(Cd["supersonic_friction"], Cl["supersonic_friction"], "Skin Friction", color="g", linestyle=linestyle_options[solver_index])
            if Cl["supersonic_combined"] != []:
                self.CL_plot.add_line(angles, Cl["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_line(angles, Cd["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_line(angles, CL_Cd["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_line(Cd["supersonic_combined"], Cl["supersonic_combined"], "Combined", color="y", linestyle=linestyle_options[solver_index])
            if Cl["inviscid"] != []:
                self.CL_plot.add_line(angles, Cl["inviscid"], "Xfoil Inviscid", color="r", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_line(angles, C_Mle["inviscid"], "viscid", color="r", linestyle=linestyle_options[solver_index])
            if Cl["viscid"] != []:
                self.CL_plot.add_line(angles, Cl["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.Cd_plot.add_line(angles, Cd["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.CL_Cd_plot.add_line(angles, CL_Cd["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.CLvCd_plot.add_line(Cd["viscid"], Cl["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])
                self.C_MLE_plot.add_line(angles, C_Mle["viscid"], "Xfoil Viscid", color="b", linestyle=linestyle_options[solver_index])

        self.command_interface.show_message(f"New data plotted for Airfoil '{self.Airfoils[solver_index].name}'", False)


    def _save_data(self, configs: list[str]):
        """
        Save simulation data for all airfoils to CSV files.

        configs:
            - ["all"]    : save both coefficients and segment conditions
            - ["coeff"]  : save only aerodynamic coefficients
            - ["cond"]   : save only segment-level conditions (P, M, T, rho)
            - or any combination, e.g. ["coeff", "cond"]
        """
        self.command_interface.show_message("Saving Data...", False)

        # Base folder: ./Results/Data/<airfoil_name>/
        filebase = os.path.join(os.getcwd(), "Results", "Data")

        for airfoil in self.Airfoils:
            airfoil_dir = os.path.join(filebase, airfoil.name)
            os.makedirs(airfoil_dir, exist_ok=True)

        if not getattr(self, "Results", None):
            self.command_interface.show_message("No data to save.", True)
            return

        lower_configs = [c.lower() for c in configs]

        # ---------- Helper: save aerodynamic coefficients ----------

        def save_coefficients():
            """
            For each dataset in self.Results, write one CSV per method

            Columns:
                AoA_deg, CL, Cd, CL/Cd, [C_MLE if available]
            """
            for dataset in self.Results:
                os.makedirs(os.path.join(filebase, dataset["airfoil"]), exist_ok=True)
                aoa_deg = dataset["AoA"]

                CL = dataset["CL"]
                Cd = dataset["Cd"]
                CL_Cd = dataset["CL_Cd"]
                C_MLE = dataset["C_MLE"]  # only has "wave" and "ackeret"

                method_to_filename = {
                    "wave":     "shock_expansion_coefficients.csv",
                    "ackeret":  "ackeret_coefficients.csv",
                    "friction": "skin_friction_coefficients.csv",
                    "combined": "combined_coefficients.csv",
                }

                for method, filename in method_to_filename.items():
                    cl_list = CL.get(method, [])
                    if not cl_list:
                        # No data for this method for this dataset
                        continue

                    cd_list = Cd.get(method, [])
                    clcd_list = CL_Cd.get(method, [])
                    c_mle_list = C_MLE.get(method, None)  # may be None

                    # Basic sanity check on lengths
                    n = len(aoa_deg)
                    if not (len(cl_list) == len(cd_list) == len(clcd_list) == n):
                        # If mismatched, skip to avoid broken CSV
                        continue

                    filepath = os.path.join(os.path.join(filebase, dataset["airfoil"]), filename)

                    with open(filepath, "w", newline="") as f:
                        writer = csv.writer(f)

                        header = ["AoA_deg", "CL", "Cd", "CL/Cd"]
                        include_c_mle = (
                            c_mle_list is not None
                            and hasattr(c_mle_list, "__len__")
                            and len(c_mle_list) == n
                        )
                        if include_c_mle:
                            header.append("C_MLE")
                        writer.writerow(header)

                        for i in range(n):
                            row = [
                                aoa_deg[i],
                                cl_list[i],
                                cd_list[i],
                                clcd_list[i],
                            ]
                            if include_c_mle:
                                row.append(c_mle_list[i])
                            writer.writerow(row)

        # ---------- Helper: save per-segment flow conditions ----------

        def save_conditions():
            pass
            # condition_to_filename = {
            #         "M":    "Segment Machs.csv",
            #         "p":    "Segment Pressures.csv",
            #         "T":    "Segment Temperatures.csv",
            #         "rho":  "Segment Densities.csv",
            # }

            # for ind, dataset in enumerate(self.Results):
            #     os.makedirs(os.path.join(filebase, dataset["airfoil"]), exist_ok=True)
            #     aoa_deg = dataset["AoA"]

            #     for result in self.Solvers[ind].Results:
            #         wave = result.wave_solution
            #         ackeret = result.ackeret_solution
            #         # need to identify the top and bottom segments and extract the state of each segment (M, P, T and rho)
                
            #     if dataset["CL"]["wave"] != []:
            #         for method, filename in condition_to_filename.items():

            #             filepath = os.path.join(os.path.join(filebase, dataset["airfoil"]), "Shock and Expansion " + filename)

            #             with open(filepath, "w", newline="") as f:
            #                 writer = csv.writer(f)

            #                 header = ["AoA_deg"] + [f'Top Segment {i}' for i in range(len(self.Airfoils[ind].top_segments))] + [f'Bottom Segment {i}' for i in range(len(self.Airfoils[ind].bottom_segments))]
            #                 writer.writerow(header)

            #                 for i in range(n):
            #                     row = [
            #                         aoa_deg[i],
            #                         cl_list[i],
            #                         cd_list[i],
            #                         clcd_list[i],
            #                     ]
            #                     if include_c_mle:
            #                         row.append(c_mle_list[i])
            #                     writer.writerow(row)


            #     if dataset["CL"]["ackeret"] != []:

        if "all" in lower_configs:
            save_coefficients()
            save_conditions()
        else:
            if "coeff" in lower_configs:
                save_coefficients()
            if "cond" in lower_configs:
                save_conditions()

        self.command_interface.show_message("Data saved to CSV files.", True)
            

    def _save_plots(self, configs : list[str]):
        self.command_interface.show_message("Saving Plots...", False)
        # Create directory to save plot
        temp = ""
        for airfoil in self.Airfoils:
            temp += airfoil.name + " & "
        temp = temp[:-3]
        filebase = os.path.join(os.getcwd(), "Results", "Plots", temp)

        if not os.path.exists(filebase):
            os.makedirs(filebase)

        name = f' Mach = {self.mach_entry.get()} P = {self.pressure_entry.get()} T = {self.temperature_entry.get()} rho = {self.density_entry.get()}'

        if len(configs) > 0:
            if configs[0].lower() == "all":
                #save all plots
                self.airfoil_plot.save(os.path.join(filebase, "Airfoil Template"))
                self.CL_plot.save(os.path.join(filebase, "CL" + name))
                self.Cd_plot.save(os.path.join(filebase, "Cd" + name))
                self.CL_Cd_plot.save(os.path.join(filebase, "CL/Cd" + name))
                self.C_MLE_plot.save(os.path.join(filebase, "C_MLE" + name))

            else:
                plot_mapping = {"airfoil": self.airfoil_plot, "cl": self.CL_plot, "cd": self.Cd_plot, "cl/cd": self.CL_Cd_plot, "c_mle": self.C_MLE_plot}
                for config in configs:
                    if config.lower() in plot_mapping.keys():
                        if config.lower() == "airfoil":
                            plot_mapping[config.lower()].save(os.path.join(filebase, "Airfoil Template"))
                        else:
                            plot_mapping[config.lower()].save(os.path.join(filebase, str(config) + name))
                    else:
                        self.command_interface.show_message(f"Config '{config}' does not exist, Skipping. Only allowed plot configs are: '-all', '-airfoil', '-Cl', '-Cd', '-Cl/Cd', and '-C_mle'", False)

            self.command_interface.show_message("All requested plots saved", True)
        
        self.command_interface.show_message("No configuration specified, please specify which plots to save.", True)

        

                        


        