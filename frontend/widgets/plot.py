import customtkinter as ctk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk


class CTkPlot(ctk.CTkFrame):
    def __init__(self, master, title="Plot", xlabel = "x", ylabel = "y", figsize=(2, 2), dpi=100, **kwargs):
        super().__init__(master, **kwargs)

        # ---- Matplotlib Figure and Axes ----
        self.fig = Figure(figsize=figsize, dpi=dpi)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_title(title)
        self.ax.set_ylabel(ylabel)
        self.ax.set_xlabel(xlabel)
        self.ax.grid(True)

        self.fig.subplots_adjust(left=0.12, bottom=0.16, top=0.92)

        self.lines = []  # store line objects

        # ---- Canvas ----
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        self.canvas.draw()
        self.canvas_widget = self.canvas.get_tk_widget()

        # ---- Toolbar Frame ----
        self.toolbar_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.toolbar_frame.pack(fill="x", padx=2, pady=2)

        # Attach toolbar to the CTkFrame
        self.toolbar = NavigationToolbar2Tk(self.canvas, self.toolbar_frame)
        for child in self.toolbar.winfo_children(): #removes shutter from toolbar
            try:
                child.configure(bd=0, padx=2, pady=0, highlightthickness=0)
            except:
                pass
        self.toolbar.update()

        # ---- Canvas in frame ----
        self.canvas_widget.pack(fill="both", expand=True)
        

    

    def clear(self):
        """Remove all lines and reset plot."""
        title, xlab, ylab = self.ax.get_title(), self.ax.get_xlabel(), self.ax.get_ylabel()
        self.ax.cla()
        self.lines.clear()
        self.ax.set_title(title)  # keep existing title
        self.ax.set_xlabel(xlab)
        self.ax.set_ylabel(ylab)
        self._refresh()
        
    def _refresh(self):
            """Redraw with fixed margins to prevent cutting."""
            self.fig.subplots_adjust(left=0.12, bottom=0.16, top=0.92)
            self.canvas.draw()

    def save(self, filename: str):
        self.fig.savefig(filename, bbox_inches="tight")

import customtkinter as ctk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk


class CTkPlot(ctk.CTkFrame):
    def __init__(self, master, title="Plot", xlabel="x", ylabel="y", figsize=(2, 2), dpi=100, **kwargs):
        super().__init__(master, **kwargs)

        # ---- Matplotlib Figure and Axes ----
        self.fig = Figure(figsize=figsize, dpi=dpi)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_title(title)
        self.ax.set_ylabel(ylabel)
        self.ax.set_xlabel(xlabel)
        self.ax.grid(True)

        self.fig.subplots_adjust(left=0.12, bottom=0.16, top=0.92)

        self.lines = []     # store normal line objects
        self.points = []    # NEW: store scatter/point objects

        # ---- Canvas ----
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        self.canvas.draw()
        self.canvas_widget = self.canvas.get_tk_widget()

        # ---- Toolbar Frame ----
        self.toolbar_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.toolbar_frame.pack(fill="x", padx=2, pady=2)

        self.toolbar = NavigationToolbar2Tk(self.canvas, self.toolbar_frame)
        for child in self.toolbar.winfo_children():
            try:
                child.configure(bd=0, padx=2, pady=0, highlightthickness=0)
            except:
                pass
        self.toolbar.update()

        self.canvas_widget.pack(fill="both", expand=True)

    # -------------------------------------------------------------------------
    # Add a line
    # -------------------------------------------------------------------------
    def add_line(self, x, y, label="_nolegend_", **plot_kwargs):
        """Add a line to the plot with optional legend support."""
        line, = self.ax.plot(x, y, label=label, **plot_kwargs)
        self.lines.append(line)

        # Update legend only if needed
        if any(l.get_label() != "_nolegend_" for l in self.lines):
            self.ax.legend()

        self._refresh()

    # -------------------------------------------------------------------------
    # Add a single point
    # -------------------------------------------------------------------------
    def add_point(self, x, y, label="_nolegend_", **plot_kwargs):
        """
        Add a single marker point to the plot.
        Example usage: plot.add_point(0.5, 0.2, color='red', marker='o')
        """

        # Use a default marker if none is supplied
        if "marker" not in plot_kwargs:
            plot_kwargs["marker"] = "o"
        if "linestyle" not in plot_kwargs:
            plot_kwargs["linestyle"] = "None"   # avoid connecting lines

        point, = self.ax.plot(x, y, label=label, **plot_kwargs)
        self.points.append(point)

        if any(p.get_label() != "_nolegend_" for p in self.points):
            self.ax.legend()

        self._refresh()

    # -------------------------------------------------------------------------
    # Clear
    # -------------------------------------------------------------------------
    def clear(self):
        title, xlab, ylab = self.ax.get_title(), self.ax.get_xlabel(), self.ax.get_ylabel()
        self.ax.cla()
        self.lines.clear()
        self.points.clear()
        self.ax.set_title(title)
        self.ax.set_xlabel(xlab)
        self.ax.set_ylabel(ylab)
        self.ax.grid(True)
        self._refresh()

    # -------------------------------------------------------------------------
    def _refresh(self):
        self.fig.subplots_adjust(left=0.12, bottom=0.16, top=0.92)
        self.canvas.draw()

    # -------------------------------------------------------------------------
    def save(self, filename: str):
        self.fig.savefig(filename, bbox_inches="tight")
