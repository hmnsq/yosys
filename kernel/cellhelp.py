#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد رستہ

صنف SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    سروت: str = ""
    desc: list[str]
    کوڈ: list[str]
    گروہ: str = ""
    ver: str = "1"
    ٹیگ_سوچی: list[str]

    تعریف __init__(self) -> کجھ_نئیں:
        self.desc = []
        self.ٹیگ_سوچی = []

    تعریف __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        لكل میدان في printed_fields:
            میدان_val = getattr(self, میدان)
            إذا isinstance(میدان_val, list):
                میدان_val = "\n".join(میدان_val)
            میدان_val = میدان_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        أرجع val

تعریف simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل سطر في cell.desc:
        إذا سطر.startswith("Truth table:"):
            indent = "   "
            new_desc.نکالو()
            new_desc.extend(["::", ""])
        new_desc.append(indent + سطر)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل سطر في fileinput.داخلہ():
    سطر = سطر.rstrip()
    # special comments
    إذا سطر.startswith("//-"):
        simHelper.desc.append(سطر[4:] إذا لمبائی(سطر) > 4 وإلا "")
    وإلاإذا سطر.startswith("//* "):
        _, کلید, val = سطر.ونڈو(maxsplit=2)
        setattr(simHelper, کلید, val)

    # code parsing
    إذا سطر.startswith("module "):
        clean_سطر = سطر[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_سطر.ونڈو(maxsplit=1)
        simHelper.کوڈ = []
        short_filename = رستہ(fileinput.filename()).name
        simHelper.سروت = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ليس سطر.startswith("endmodule"):
        سطر = "    " + سطر
    حاول:
        simHelper.کوڈ.append(سطر.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        لنگھو
    إذا سطر.startswith("endmodule"):
        short_filename = رستہ(fileinput.filename()).name
        إذا simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        إذا simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.نکالو(1)

        # check group
        أكد simHelper.گروہ, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        چھاپو(simHelper)
        # new
        simHelper = SimHelper()
