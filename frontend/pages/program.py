
import os

import math
from customtkinter import *
from tkinter import Menu, filedialog, StringVar
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from utilities import ui_messages
from widgets.plot import CTkPlot
from widgets.command_prompt import CTkCommand
import sonicfoil_backend as SonicFoil



#self.master is root!
#ADD XCP AND YCP PLOTS

class ProgramPage(CTkFrame):
    def __init__(self, master: CTk, **kwargs):
        super().__init__(master, **kwargs)

        self.Solver = None

        self.grid_columnconfigure((0 , 1 , 2, 3, 4), weight=1)
        self.grid_rowconfigure((0, 1, 2, 3), weight=1)
        
        #Configure bar
        self.menu = Menu(self.master)
        self.file_menu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='File', menu=self.file_menu)
        self.file_menu.add_command(label='New', command=None) # add command
        self.file_menu.add_command(label='Open...', command=None) # add command
        self.file_menu.add_separator()
        self.file_menu.add_command(label='Settings', command=None)
        self.file_menu.add_command(label='Exit', command=self.master.quit)
        helpmenu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='Help', menu=helpmenu)
        helpmenu.add_command(label='About', command=None) # add command
        helpmenu.add_command(label='Guide', command=None) # add command

        self.master.configure(menu=self.menu)

        ##########################################
        #load airfoil frame
        self.load_airfoil_frm = CTkFrame(self)
        self.load_airfoil_frm.grid_columnconfigure((0 , 1 , 2), weight=1)
        self.load_airfoil_frm.grid_rowconfigure((0, 1, 2), weight=1)

        self.load_airfoil_title_lbl = CTkLabel(self.load_airfoil_frm, text="Load Airfoil", font=("Arial", 16), anchor="center")
        self.load_airfoil_file_lbl = CTkLabel(self.load_airfoil_frm, text="File:", font=("Arial", 12), anchor="e")
        self.load_airfoil_file_opt = CTkOptionMenu(self.load_airfoil_frm, font=("Arial", 12), values= ["..."] + self._get_available_projects())
        self.load_airfoil_browse_file_btn = CTkButton(self.load_airfoil_frm, text="Browse", font=("Arial", 12), command=self._browse_files, anchor="w")
        self.load_airfoil_load_btn = CTkButton(self.load_airfoil_frm, text="Load Airfoil", font=("Arial", 14), command=self._load_airfoil)

        self.load_airfoil_title_lbl.grid(row=0, column=0, columnspan=3, padx=5, pady=5, sticky="nsew")
        self.load_airfoil_file_lbl.grid(row=1, column=0, padx=(5, 1), pady=5, sticky="ew")
        self.load_airfoil_file_opt.grid(row=1, column=1, padx=5, pady=5, sticky = "ew")
        self.load_airfoil_browse_file_btn.grid(row=1, column=2, padx=(1, 5), pady=5, sticky="w")
        self.load_airfoil_load_btn.grid(row=2, column=1, padx=5, pady=5, sticky = "ew")

        self.load_airfoil_frm.grid(row=0, column=0, columnspan=2, padx=5, pady=5, sticky="nsew")

        ###########################
        # input frame
        self.inputs_frm = CTkFrame(self)
        self.inputs_frm.grid_columnconfigure((0, 1, 2, 3), weight=1)
        self.inputs_frm.grid_rowconfigure((0, 1, 2, 3, 4), weight=1)

        self.input_lbl = CTkLabel(self.inputs_frm, text="Solver Inputs", font=("Arial", 16), anchor="center")
        self.input_lbl.grid(row=0, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

        self.methods_frm = CTkFrame(self.inputs_frm)
        self.methods_frm.grid_columnconfigure((0, 1, 2, 3), weight=1)
        self.methods_frm.grid_rowconfigure(0, weight=1)
        self.method_lbl = CTkLabel(self.methods_frm, text="Methods:", font=("Arial", 14), anchor="e")
        self.wave_var = StringVar(value="off")
        self.ackeret_var = StringVar(value="off")
        self.friction_var = StringVar(value="off")
        self.wave_method_radbtn = CTkCheckBox(self.methods_frm, text="Shock/Expansion", command=None, variable= self.wave_var, onvalue="on", offvalue="off")
        self.ackeret_method_radbtn = CTkCheckBox(self.methods_frm, text="Ackeret", command=None, variable= self.ackeret_var, onvalue="on", offvalue="off")
        self.friction_switch = CTkSwitch(self.methods_frm, text="Skin Friction", command=None, variable= self.friction_var, onvalue="on", offvalue="off")

        self.method_lbl.grid(row=0, column=0, padx=(10, 5), pady=5, sticky="nsew") # should maybe put these in there own frame
        self.wave_method_radbtn.grid(row=0, column=1, padx=(5, 5), pady=5, sticky="nsew")
        self.ackeret_method_radbtn.grid(row=0, column=2, padx=(0, 5), pady=5, sticky="nsew")
        self.friction_switch.grid(row=0, column=3, padx=(5, 10), pady=5, sticky="nsew")
        self.methods_frm.grid(row=1, column=0, columnspan=4, padx=10, pady=10, sticky="nsew")

        self.flow_conditions_input_frm = CTkFrame(self.inputs_frm)
        self.flow_conditions_input_frm.grid_columnconfigure((0, 1, 2, 3) , weight=1)
        self.flow_conditions_input_frm.grid_rowconfigure((0, 1, 2), weight=1)
        self.freestream_lbl = CTkLabel(self.flow_conditions_input_frm, text="Freestream Flow Conditions", font=("Arial", 14), anchor="center")
        self.mach_lbl = CTkLabel(self.flow_conditions_input_frm, text="Mach Number:", font=("Arial", 12), anchor="e")
        self.mach_entry = CTkEntry(self.flow_conditions_input_frm, placeholder_text="1.0")
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
        self.flow_conditions_input_frm.grid(row=2, column=0, columnspan=4, padx=5, pady=5, sticky="nsew")

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
        self.angle_range_step_size_lbl = CTkLabel(self.angle_range_tab, text="Step Size:", font=("Arial", 12), anchor="e")
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

        self.inputs_frm.grid(row=1, column=0, rowspan=3, columnspan=2, padx=5, pady=5, sticky="nsew")

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

        self.C_MLE_tab = self.plots.add("C_MLE")
        self.C_MLE_plot = CTkPlot(self.C_MLE_tab, "Coefficient of Moment About Leading Edge", "alpha (degrees)", "C_MLE", dpi=150)

        self.airfoil_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.CL_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.Cd_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.CL_Cd_plot.pack(padx=0, pady=0, fill="both", expand=True)
        self.C_MLE_plot.pack(padx=0, pady=0, fill="both", expand=True)

        self.plots.grid(row=0, column=0, sticky="nsew")
        self.plots_frm.grid(row=0, column=2, rowspan=3, columnspan=3, padx=5, pady=5, sticky="nsew")

        # text output frame
        self.text_output_frm = CTkFrame(self)
        
        self.command_interface = CTkCommand(self.text_output_frm)
        self.command_interface.pack(padx=10, pady=10, fill="both")

        self.text_output_frm.grid(row=3, column=2, columnspan=3, padx=5, pady=5, sticky="nsew")

        # self.configure_bar = CTkFrame(self)
        # self.grid_columnconfigure((0 , 1 , 2, 3), weight=1)
        # self.grid_rowconfigure((0), weight=1)


        # self.configure_bar.grid(row=0, column=0, columnspan=4, padx=1, pady=1, sticky="nsew")

    def _get_available_projects(self) -> list:
        cur_dir = os.path.join(os.getcwd(), "Airfoil Files")
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
        filename = filedialog.askopenfilename(initialdir = "/", title = "Select a File", filetypes = (("Airfoil files", "*.dat*"),("all files", "*.*")))
        self.load_airfoil_file_opt.set(filename)
        
    def _load_airfoil(self) -> None:
        filename = self.load_airfoil_file_opt.get()
        if os.path.exists(filename):
            if not filename.endswith(".dat"):
                ui_messages.gui_error(f'Entered filename "{filename}" is not the proper ".dat" format')
                return
            self.Airfoil = SonicFoil.Airfoil(filename)
            self.Solver = SonicFoil.Solver(self.Airfoil)
            self._update_airfoil_plot()
            self.command_interface.show_message(f'Loaded in Airfoil: "{self.Airfoil.name}" from file: {filename}', True)
        elif os.path.exists(os.path.join(os.getcwd(), "Airfoil Files", filename)):
            self.Airfoil = SonicFoil.Airfoil(os.path.join(os.getcwd(), "Airfoil Files", filename))
            self.Solver = SonicFoil.Solver(self.Airfoil)
            self._update_airfoil_plot()
            self.command_interface.show_message(f'Loaded in Airfoil: "{self.Airfoil.name}" from file: {os.path.join(os.getcwd(), "Airfoil Files", filename)}', True)
        else:
            ui_messages.gui_error(f'Entered filename "{filename}" does not exist')
            return


    def _update_airfoil_plot(self) -> None:
        self.airfoil_plot.clear()
        for segment in self.Airfoil.top_segments:
            x = [segment.start.x, segment.end.x]
            y = [segment.start.y, segment.end.y]
            self.airfoil_plot.add_line(x, y, "_nolegend_", color='black', linestyle="solid", marker=None)
        for segment in self.Airfoil.bottom_segments:
            x = [segment.start.x, segment.end.x]
            y = [segment.start.y, segment.end.y]
            self.airfoil_plot.add_line(x, y, "_nolegend_", color='black', linestyle="solid", marker=None)
        self.airfoil_plot.ax.set_aspect('equal', adjustable='datalim')
        self.airfoil_plot._refresh()

    def _clear_results_plots(self) -> None:
        self.CL_plot.clear()
        self.Cd_plot.clear()
        self.CL_Cd_plot.clear()
        self.C_MLE_plot.clear()
        self.command_interface.show_message("Result plots cleared", False)

    def _run(self) -> None:
        self.command_interface.show_message("Running Solver...", False)
        method = ""
        if self.wave_var.get() == "on" and self.ackeret_var.get() == "on":
            method += "b"
        elif self.wave_var.get() == "on":
            method += "w"
        elif self.ackeret_var.get() == "on":
            method += "a"
        else:
            ui_messages.gui_error("Please select a solver method to solve the pressure distribution, cannot run without a selection.")
            return

        if self.friction_var.get() == "on":
            if method == "a":
                ui_messages.gui_popup("Cannot solve skin friciton using ackeret method results. No friction result will be produced.")
            else:
                method += "d"

        mach = float(self.mach_entry.get()) if self.mach_entry.get() != '' else 1.0
        pressure = float(self.pressure_entry.get()) if self.pressure_entry.get() != '' else 101325
        density = float(self.density_entry.get()) if self.density_entry.get() != '' else 1.225
        temperature = float(self.temperature_entry.get()) if self.temperature_entry.get() != '' else 273.15

        if mach <= 1 or mach > 5:
            ui_messages.gui_error("Mach number is out of allowbale regime. Mach Number must be within 1 < M < 10")
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

        if self.Solver is None:
            ui_messages.gui_error("Please load in an Airfoil to use the solver")
            return

        self.Solver.solve_range(method, AoA, mach, pressure, temperature, density)
        self.command_interface.show_message("Solver Complete Running", False)
        self._update_result_plots(self.Solver.Results, AoA)

    def _update_result_plots(self, results: list, AoA: list) -> None:

        self._clear_results_plots()

        Cl = {"wave": [], "ackeret": [], "friction": [], "combined": []}
        Cd = {"wave": [], "ackeret": [], "friction": [], "combined": []}
        CL_Cd = {"wave": [], "ackeret": [], "friction": [], "combined": []}
        C_Mle = {"wave": [], "ackeret": []} # curently no Cmle for friction
        #currently no xcp and ycp

        for result in results:
            wave = result.wave_solution
            ackeret = result.ackeret_solution
            friction = result.skin_friction_solution
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
            if friction is not None:
                Cl["friction"].append(friction.Forces.CL)
                Cd["friction"].append(friction.Forces.Cd)
                CL_Cd["friction"].append(friction.Forces.CL_Cd)
        
        if Cl["wave"] != [] and Cl["friction"] != []:
            for i in range(len(Cl["wave"])):
                Cl["combined"].append(Cl["wave"][i]+Cl["friction"][i])
                Cd["combined"].append(Cd["wave"][i]+Cd["friction"][i])
                CL_Cd["combined"].append(CL_Cd["wave"][i]+CL_Cd["friction"][i])

        self.Results = {"CL": Cl, "Cd": Cd, "CL_Cd": CL_Cd, "C_MLE": C_Mle}

        angles = [rad * 180 / math.pi for rad in AoA]

        if Cl["wave"] != []:
            self.CL_plot.add_line(angles, Cl["wave"], "Shock/Expansion")
            self.Cd_plot.add_line(angles, Cd["wave"], "Shock/Expansion")
            self.CL_Cd_plot.add_line(angles, CL_Cd["wave"], "Shock/Expansion")
            self.C_MLE_plot.add_line(angles, C_Mle["wave"], "Shock/Expansion")
        if Cl["ackeret"] != []:
            self.CL_plot.add_line(angles, Cl["ackeret"], "Ackeret")
            self.Cd_plot.add_line(angles, Cd["ackeret"], "Ackeret")
            self.CL_Cd_plot.add_line(angles, CL_Cd["ackeret"], "Ackeret")
            self.C_MLE_plot.add_line(angles, C_Mle["ackeret"], "Ackeret")
        if Cl["friction"] != []:
            self.CL_plot.add_line(angles, Cl["friction"], "Skin Friction")
            self.Cd_plot.add_line(angles, Cd["friction"], "Skin Friction")
            self.CL_Cd_plot.add_line(angles, CL_Cd["friction"], "Skin Friction")
        if Cl["combined"] != []:
            self.CL_plot.add_line(angles, Cl["combined"], "Combined")
            self.Cd_plot.add_line(angles, Cd["combined"], "Combined")
            self.CL_Cd_plot.add_line(angles, CL_Cd["combined"], "Combined")

        self.command_interface.show_message("New data plotted", True)