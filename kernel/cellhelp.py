#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد مسیر

صنف SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    منبع: str = ""
    desc: list[str]
    کد: list[str]
    گروه: str = ""
    ver: str = "1"
    برچسبها: list[str]

    تعریف __init__(self) -> هیچ:
        self.desc = []
        self.برچسبها = []

    تعریف __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        لكل فیلد في printed_fields:
            فیلد_val = getattr(self, فیلد)
            إذا isinstance(فیلد_val, list):
                فیلد_val = "\n".join(فیلد_val)
            فیلد_val = فیلد_val.strip()
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
            new_desc.برداشت()
            new_desc.extend(["::", ""])
        new_desc.append(indent + سطر)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل سطر في fileinput.درون_داد():
    سطر = سطر.rstrip()
    # special comments
    إذا سطر.startswith("//-"):
        simHelper.desc.append(سطر[4:] إذا طول(سطر) > 4 وإلا "")
    وإلاإذا سطر.startswith("//* "):
        _, کلید, val = سطر.تقسیم(maxsplit=2)
        setattr(simHelper, کلید, val)

    # code parsing
    إذا سطر.startswith("module "):
        clean_سطر = سطر[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_سطر.تقسیم(maxsplit=1)
        simHelper.کد = []
        short_filename = مسیر(fileinput.filename()).name
        simHelper.منبع = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ليس سطر.startswith("endmodule"):
        سطر = "    " + سطر
    حاول:
        simHelper.کد.append(سطر.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        بگذر
    إذا سطر.startswith("endmodule"):
        short_filename = مسیر(fileinput.filename()).name
        إذا simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        إذا simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.برداشت(1)

        # check group
        أكد simHelper.گروه, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        چاپ(simHelper)
        # new
        simHelper = SimHelper()
