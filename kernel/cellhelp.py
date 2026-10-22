#!/usr/bin/env python3

سے __future__ درآمد annotations
درآمد fileinput
درآمد json
سے pathlib درآمد راستہ

جماعت SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    ماخذ: str = ""
    desc: list[str]
    رمز: list[str]
    گروہ: str = ""
    ver: str = "1"
    لیبل_فہرست: list[str]

    تعریف __init__(self) -> کچھ_نہیں:
        self.desc = []
        self.لیبل_فہرست = []

    تعریف __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        برائے میدان میں printed_fields:
            میدان_val = getattr(self, میدان)
            اگر isinstance(میدان_val, list):
                میدان_val = "\n".join(میدان_val)
            میدان_val = میدان_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        واپس val

تعریف simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    برائے سطر میں cell.desc:
        اگر سطر.startswith("Truth table:"):
            indent = "   "
            new_desc.نکالو()
            new_desc.extend(["::", ""])
        new_desc.append(indent + سطر)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

برائے سطر میں fileinput.داخلہ():
    سطر = سطر.rstrip()
    # special comments
    اگر سطر.startswith("//-"):
        simHelper.desc.append(سطر[4:] اگر لمبائی(سطر) > 4 ورنہ "")
    وگرنہ_اگر سطر.startswith("//* "):
        _, کلید, val = سطر.بانٹو(maxsplit=2)
        setattr(simHelper, کلید, val)

    # code parsing
    اگر سطر.startswith("module "):
        clean_سطر = سطر[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_سطر.بانٹو(maxsplit=1)
        simHelper.رمز = []
        short_filename = راستہ(fileinput.filename()).name
        simHelper.ماخذ = f'{short_filename}:{fileinput.filelineno()}'
    وگرنہ_اگر نہیں سطر.startswith("endmodule"):
        سطر = "    " + سطر
    کوشش:
        simHelper.رمز.append(سطر.replace("\t", "    "))
    استثنا AttributeError:
        # no module definition, ignore line
        گزرو
    اگر سطر.startswith("endmodule"):
        short_filename = راستہ(fileinput.filename()).name
        اگر simHelper.ver == "1" اور short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        اگر simHelper.desc اور simHelper.ver == "1" اور short_filename == "simlib.v" اور simHelper.desc[1].startswith('    '):
            simHelper.desc.نکالو(1)

        # check group
        أكد simHelper.گروہ, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        چھاپو(simHelper)
        # new
        simHelper = SimHelper()
