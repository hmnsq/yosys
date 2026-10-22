#!/usr/bin/env python3

از __future__ واردکن annotations
واردکن fileinput
واردکن json
از pathlib واردکن Path

رده SimHelper:
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
        برای field در printed_fields:
            field_val = getattr(self, field)
            اگر isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        بازگشت val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    برای سطر در cell.desc:
        اگر سطر.startswith("Truth table:"):
            indent = "   "
            new_desc.برداشت()
            new_desc.extend(["::", ""])
        new_desc.append(indent + سطر)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

برای سطر در fileinput.درون_داد():
    سطر = سطر.rstrip()
    # special comments
    اگر سطر.startswith("//-"):
        simHelper.desc.append(سطر[4:] اگر len(سطر) > 4 وگرنه "")
    وگرنهاگر سطر.startswith("//* "):
        _, کلید, val = سطر.split(maxsplit=2)
        setattr(simHelper, کلید, val)

    # code parsing
    اگر سطر.startswith("module "):
        clean_سطر = سطر[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_سطر.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    وگرنهاگر نه سطر.startswith("endmodule"):
        سطر = "    " + سطر
    تلاش:
        simHelper.code.append(سطر.replace("\t", "    "))
    استثنا AttributeError:
        # no module definition, ignore line
        pass
    اگر سطر.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        اگر simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        اگر simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.برداشت(1)

        # check group
        أكد simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
