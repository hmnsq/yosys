#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد لار

صنف SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    سرچینه: str = ""
    desc: list[str]
    کوډ: list[str]
    ډله: str = ""
    ver: str = "1"
    نښانونه: list[str]

    تعریف __init__(self) -> هیڅ:
        self.desc = []
        self.نښانونه = []

    تعریف __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        لكل ډګر في printed_fields:
            ډګر_val = getattr(self, ډګر)
            إذا isinstance(ډګر_val, list):
                ډګر_val = "\n".join(ډګر_val)
            ډګر_val = ډګر_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        أرجع val

تعریف simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل کرښه في cell.desc:
        إذا کرښه.startswith("Truth table:"):
            indent = "   "
            new_desc.راوباسه()
            new_desc.extend(["::", ""])
        new_desc.append(indent + کرښه)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل کرښه في fileinput.ننوت():
    کرښه = کرښه.rstrip()
    # special comments
    إذا کرښه.startswith("//-"):
        simHelper.desc.append(کرښه[4:] إذا اوږدوالی(کرښه) > 4 وإلا "")
    وإلاإذا کرښه.startswith("//* "):
        _, کلی, val = کرښه.وېشل(maxsplit=2)
        setattr(simHelper, کلی, val)

    # code parsing
    إذا کرښه.startswith("module "):
        clean_کرښه = کرښه[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_کرښه.وېشل(maxsplit=1)
        simHelper.کوډ = []
        short_filename = لار(fileinput.filename()).name
        simHelper.سرچینه = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ليس کرښه.startswith("endmodule"):
        کرښه = "    " + کرښه
    حاول:
        simHelper.کوډ.append(کرښه.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        تیرشه
    إذا کرښه.startswith("endmodule"):
        short_filename = لار(fileinput.filename()).name
        إذا simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        إذا simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.راوباسه(1)

        # check group
        أكد simHelper.ډله, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        چاپ(simHelper)
        # new
        simHelper = SimHelper()
