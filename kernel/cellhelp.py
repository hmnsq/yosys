#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد Path

صنف SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    source: str = ""
    desc: list[str]
    code: list[str]
    group: str = ""
    ver: str = "1"
    tags: list[str]

    def __init__(self) -> None:
        self.desc = []
        self.tags = []

    def __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        لكل field في printed_fields:
            field_val = getattr(self, field)
            إذا isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        أرجع val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل هێڵ في cell.desc:
        إذا هێڵ.startswith("Truth table:"):
            indent = "   "
            new_desc.دەرهێنان()
            new_desc.extend(["::", ""])
        new_desc.append(indent + هێڵ)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل هێڵ في fileinput.تێخستن():
    هێڵ = هێڵ.rstrip()
    # special comments
    إذا هێڵ.startswith("//-"):
        simHelper.desc.append(هێڵ[4:] إذا len(هێڵ) > 4 وإلا "")
    وإلاإذا هێڵ.startswith("//* "):
        _, کلیل, val = هێڵ.split(maxsplit=2)
        setattr(simHelper, کلیل, val)

    # code parsing
    إذا هێڵ.startswith("module "):
        clean_هێڵ = هێڵ[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_هێڵ.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ليس هێڵ.startswith("endmodule"):
        هێڵ = "    " + هێڵ
    حاول:
        simHelper.code.append(هێڵ.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        pass
    إذا هێڵ.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        إذا simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        إذا simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.دەرهێنان(1)

        # check group
        أكد simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
