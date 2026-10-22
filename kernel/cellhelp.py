#!/usr/bin/env python3

from __future__ आयात annotations
आयात fileinput
आयात json
from pathlib आयात Path

श्रेणी SimHelper:
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
        क्रम field in printed_fields:
            field_val = getattr(self, field)
            अगर isinstance(field_val, list):
                field_val = "\n".मेल(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        वापस val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    क्रम पंक्ति in cell.desc:
        अगर पंक्ति.startswith("Truth table:"):
            indent = "   "
            new_desc.निकालो()
            new_desc.extend(["::", ""])
        new_desc.append(indent + पंक्ति)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

क्रम पंक्ति in fileinput.निवेश():
    पंक्ति = पंक्ति.rstrip()
    # special comments
    अगर पंक्ति.startswith("//-"):
        simHelper.desc.append(पंक्ति[4:] अगर len(पंक्ति) > 4 अन्यथा "")
    elif पंक्ति.startswith("//* "):
        _, कुंजी, val = पंक्ति.split(maxsplit=2)
        setattr(simHelper, कुंजी, val)

    # code parsing
    अगर पंक्ति.startswith("module "):
        clean_पंक्ति = पंक्ति[7:].प्रतिस्थापन("\\", "").प्रतिस्थापन(";", "")
        simHelper.name, simHelper.ports = clean_पंक्ति.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    elif not पंक्ति.startswith("endmodule"):
        पंक्ति = "    " + पंक्ति
    प्रयास:
        simHelper.code.append(पंक्ति.प्रतिस्थापन("\t", "    "))
    except AttributeError:
        # no module definition, ignore line
        pass
    अगर पंक्ति.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        अगर simHelper.ver == "1" and short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        अगर simHelper.desc and simHelper.ver == "1" and short_filename == "simlib.v" and simHelper.desc[1].startswith('    '):
            simHelper.desc.निकालो(1)

        # check group
        assert simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
