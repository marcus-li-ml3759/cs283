1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client waits until it sees a special marker at the end of the data from the server. It keeps reading data
into a buffer until this marker appears. Other methods include sending a header that tells the total size of the
message or using a unique delimiter to mark the end.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

To handle this, a network shell protocol must add clear start and end markers for each command. Without these
markers, messages might run together or split apart, leading to errors in interpreting the commands.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols keep information about previous interactions, so eacch new message can use that stored
information. Stateless protocols treat every message as independent, with no memory of past messages. Stateful
systems can handle complex interactions while stateless ones are simpler.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It could be used because it is faster and has lower overhead than TCP. This makes UDP a good choice for applications
where speed is more important than reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the Berkeley sockets API to let applications use network communications.
