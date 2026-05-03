python
import gdb
import gdb.printing

print("---> Loading Pikuma GDB pretty printers (v2)...")

class Matrix4Printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            val = self.val
            # Automatically handle pointers and references from VS Code
            if val.type.code == gdb.TYPE_CODE_PTR or val.type.code == gdb.TYPE_CODE_REF:
                val = val.dereference()
                
            m = val['m']
            res = "\n"
            for i in range(4):
                # %g naturally hides decimal places if the number is a whole integer!
                res += "| %7g, %7g, %7g, %7g |\n" % (float(m[i][0]), float(m[i][1]), float(m[i][2]), float(m[i][3]))
            return res
        except Exception as e:
            return f"Matrix4Printer Error: {str(e)}"
            
    def display_hint(self):
        return 'string'

class Vector3fPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            val = self.val
            if val.type.code == gdb.TYPE_CODE_PTR or val.type.code == gdb.TYPE_CODE_REF:
                val = val.dereference()
            return "[ %g, %g, %g ]" % (float(val['x']), float(val['y']), float(val['z']))
        except Exception as e:
            return f"Vector3fPrinter Error: {str(e)}"
            
    def display_hint(self):
        return 'string'

class Vector4fPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            val = self.val
            if val.type.code == gdb.TYPE_CODE_PTR or val.type.code == gdb.TYPE_CODE_REF:
                val = val.dereference()
            return "[ %g, %g, %g, %g ]" % (float(val['x']), float(val['y']), float(val['z']), float(val['w']))
        except Exception as e:
            return f"Vector4fPrinter Error: {str(e)}"
            
    def display_hint(self):
        return 'string'

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("pikuma")
    # Matches 'Matrix4', 'class Matrix4', 'Geometry::Matrix4', etc.
    pp.add_printer('Matrix4', '^.*Matrix4.*$', Matrix4Printer)
    pp.add_printer('Vector3f', '^.*Vector3f.*$', Vector3fPrinter)
    pp.add_printer('Vector4f', '^.*Vector4f.*$', Vector4fPrinter)
    return pp

gdb.printing.register_pretty_printer(None, build_pretty_printer(), replace=True)
end