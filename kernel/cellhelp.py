#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد رستو

صنف SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    ذريعو: str = ""
    desc: list[str]
    ڪوڊ: list[str]
    گروهه: str = ""
    ver: str = "1"
    ٽيگ_فهرست: list[str]

    وصف __init__(self) -> ڪجهه_نه:
        self.desc = []
        self.ٽيگ_فهرست = []

    وصف __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        لكل ميدان في printed_fields:
            ميدان_val = getattr(self, ميدان)
            إذا isinstance(ميدان_val, list):
                ميدان_val = "\n".join(ميدان_val)
            ميدان_val = ميدان_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        أرجع val

وصف simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل سٽ في cell.desc:
        إذا سٽ.startswith("Truth table:"):
            indent = "   "
            new_desc.ڪڍو()
            new_desc.extend(["::", ""])
        new_desc.append(indent + سٽ)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل سٽ في fileinput.داخلا():
    سٽ = سٽ.rstrip()
    # special comments
    إذا سٽ.startswith("//-"):
        simHelper.desc.append(سٽ[4:] إذا ڊيگهه(سٽ) > 4 وإلا "")
    وإلاإذا سٽ.startswith("//* "):
        _, ڪنجي, val = سٽ.ورهايو(maxsplit=2)
        setattr(simHelper, ڪنجي, val)

    # code parsing
    إذا سٽ.startswith("module "):
        clean_سٽ = سٽ[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_سٽ.ورهايو(maxsplit=1)
        simHelper.ڪوڊ = []
        short_filename = رستو(fileinput.filename()).name
        simHelper.ذريعو = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ليس سٽ.startswith("endmodule"):
        سٽ = "    " + سٽ
    حاول:
        simHelper.ڪوڊ.append(سٽ.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        لنگهه
    إذا سٽ.startswith("endmodule"):
        short_filename = رستو(fileinput.filename()).name
        إذا simHelper.ver == "1" و short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        إذا simHelper.desc و simHelper.ver == "1" و short_filename == "simlib.v" و simHelper.desc[1].startswith('    '):
            simHelper.desc.ڪڍو(1)

        # check group
        أكد simHelper.گروهه, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        ڇاپيو(simHelper)
        # new
        simHelper = SimHelper()
