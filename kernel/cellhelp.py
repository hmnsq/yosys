#!/usr/bin/env python3

తః __future__ దిగుమతి annotations
దిగుమతి fileinput
దిగుమతి json
తః pathlib దిగుమతి మార్గం

తరగతి SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    ఆధారం: str = ""
    desc: list[str]
    సంకేతం_2: list[str]
    సమూహం: str = ""
    ver: str = "1"
    ట్యాగులు: list[str]

    నిర్వచనం __init__(self) -> ఏమీలేదు:
        self.desc = []
        self.ట్యాగులు = []

    నిర్వచనం __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        కొరకు క్షేత్రం లో printed_fields:
            క్షేత్రం_val = getattr(self, క్షేత్రం)
            ఒకవేళ isinstance(క్షేత్రం_val, list):
                క్షేత్రం_val = "\n".join(క్షేత్రం_val)
            క్షేత్రం_val = క్షేత్రం_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        తిరిగి val

నిర్వచనం simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    కొరకు పంక్తి లో cell.desc:
        ఒకవేళ పంక్తి.startswith("Truth table:"):
            indent = "   "
            new_desc.తీయి()
            new_desc.extend(["::", ""])
        new_desc.append(indent + పంక్తి)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

కొరకు పంక్తి లో fileinput.ఆదానం():
    పంక్తి = పంక్తి.rstrip()
    # special comments
    ఒకవేళ పంక్తి.startswith("//-"):
        simHelper.desc.append(పంక్తి[4:] ఒకవేళ పొడవు(పంక్తి) > 4 లేకపోతే "")
    లేదావేళ పంక్తి.startswith("//* "):
        _, కీలకం, val = పంక్తి.విభజించు(maxsplit=2)
        setattr(simHelper, కీలకం, val)

    # code parsing
    ఒకవేళ పంక్తి.startswith("module "):
        clean_పంక్తి = పంక్తి[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_పంక్తి.విభజించు(maxsplit=1)
        simHelper.సంకేతం_2 = []
        short_filename = మార్గం(fileinput.filename()).name
        simHelper.ఆధారం = f'{short_filename}:{fileinput.filelineno()}'
    లేదావేళ కాదు పంక్తి.startswith("endmodule"):
        పంక్తి = "    " + పంక్తి
    ప్రయత్నించు:
        simHelper.సంకేతం_2.append(పంక్తి.replace("\t", "    "))
    మినహాయింపు AttributeError:
        # no module definition, ignore line
        దాటు
    ఒకవేళ పంక్తి.startswith("endmodule"):
        short_filename = మార్గం(fileinput.filename()).name
        ఒకవేళ simHelper.ver == "1" మరియు short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ఒకవేళ simHelper.desc మరియు simHelper.ver == "1" మరియు short_filename == "simlib.v" మరియు simHelper.desc[1].startswith('    '):
            simHelper.desc.తీయి(1)

        # check group
        ప్రతిజ్ఞా simHelper.సమూహం, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        ముద్రించు(simHelper)
        # new
        simHelper = SimHelper()
